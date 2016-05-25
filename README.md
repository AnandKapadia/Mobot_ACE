# Mobot_ACE

##Summary
This robot worked exceptionally well at night with the back panel of IR sensors proving a useful correcting mechanism for the front IR panels. The hills on the outdoor linefollowing course was also handled well after tuning of parameters, and the speed of the robot was relatively fast. Hacking the TRAXIS RC car with an arduino was a very managable task and a high level of control was aquired from the chassis. 

The disadvantage to this robot design was IR sensitivity in direct sunlight. Due to non-optimal choice in IR modules and inability to create a robust shroud that could handle the bumps and incline of the course, the sensors were often washed out in the sun. Error correcting code was not sufficent to overcome this issue. 

Future suggestions include adding a camera to the existing IR-based algorithm for more robust linefollowing, and looking into using cross correlation to improve IR sensor readings. 

![alt text](https://github.com/AnandKapadia/Mobot_ACE/mobot1.jpg)
![alt text](https://github.com/AnandKapadia/Mobot_ACE/mobot2.jpg)


## Bill of Materials

The Bill of Materials (BOM) shall be updated with whatever parts
were purchased including prototyping and final build. bom.ods is
in the OpenDocument format so you'll need libreoffice or openoffice,
etc. (Does Word support it yet? Probably. Don't know, I don't use it.) 

## Mobot Control System Requirements

#### Features (by priority):

1. Wirelessly Retrieve Values
  * Arduino presents a set of key-value pairs
  * Values can be requested by the PC by name/index at any time
  * Values can be streamed at intervals or when updated

2. Wirelessly Overwrite Values
  * PC can supply a new value for a key

3. Remote Control Using Gamepad
  * 2) occurs sufficiently fast and with appropriate data 
  (ie. motorspeed and steering) such that the mobot can be remote-controlled
  * 1) and 2) can continue to occur while manual control is enabled,
  allowing us to record data from manual tests. 

#### Interface Options - PC side

1. GUI, such as Python Tk or JAVA
2. CLI, with a simple set of built-in commands defined (mobotsh)

####Implementation Details

1. A serial (UART) line is established between the mobot (arduino) and the pc using XBee Radios.
Communication is full-duplex (separate RX and TX lines, two way).
2. Messages between the PC and Mobot are packetized. Packets contain a header followed by a sequence of bytes. Several message types are defined for the different types of data exchanges, i.e:
  1. "Ping" or "ID" packet
    * Causes the mobot to identify itself 
    * Tests whether the mobot is present
  2. "Request Value Table" packet
    * Mobot responds with a list of the currently defined keys and values.
    * Key/value pairs may be stored in an array on the Mobot rather than a hash table. This would eliminiate the need to send the key strings through the network repeatedly.
  3. "Request value n" packet
    * Mobot responds with the value at array index n.
  4. "Set value n to x" packet
    * Mobot sets A[n] = x.
    * A confirmation packet is sent in response (may not be wise for frequently updated values, such as the manual control parameters).
  5. "Configure Test Output" packet
    * The PC sends the indexes of the values to be streamed during a test.
    * Once the test is started, the mobot outputs this subset of values
  continuously, at intervals, or every time a value changes.
  6. "Set Mode" packet
    * Packet contains the new mode ID, for example
    0 = IDLE, 1 = RUN, 2 = TEST
    * Different modes determine which values are written to output,
    for example Mode = 2 would output the values set up with
    "Configure Test Output" packets.
    * Mode = 1 might disable all RF communication, because we
    don't want to get disqualified for this during the competition.
    * The mobot state machine (main loop) would also transistion between
    modes naturally, i.e. RUN -> IDLE or TEST -> IDLE when we reach the end
    of the course.
3. Response Packets
  1. Communication is full-duplex, so all "Request" type packets sent from
  the PC are transmitted from TX (PC) -> RX (Mobot).
  2. Packets sent from the mobot to the PC are "Response" type, and are
  transmitted from TX (Mobot) -> RX (PC).
  3. All "Response" packets should have a matching "Request". The responses
  should contain a header that identifies which packet was the request.
4. User Interface
  1. The PC program can write the output of a test to a .csv file. This
  would allow us to view and graph our test data to more easily tune our
  algorithms.
  2. The PC can display values on request by the user. If we see the mobot
  do something weird, we can immediately request any of the values in the
  key/value array.
  3. The PC can output values continuously, similar to our previous printf
  approach but cleaner. Using the "Configure Test Output" packet, you would
  specify which values would be displayed.
  4. Manual control of the mobot is allowed at any time. This is implemented
  via a "Manual Control Enable" boolean defined in the key/value array, along
  with "Steering" and "Throttle" key/value pairs. Manual control is performed
  by first setting "Manual Control Enable" = 1, then writing to "Throttle" and
  "Steering" rapidly using the input from a game controller.
  5. A minimal number of hardware human interface components exist on the mobot,
  such as start/stop buttons. These are used to start or stop tests in emergencies
  or when operating without RF communication.
   

## Example Use Cases

1. Setting the mobot gate configuration
 * Given n decision points at the end of the mobot course,
 we define bit vectors in the key/value array with at least n bits.
 * On the PC side, we write values to the bit vectors. '1' means turn left,
 '0' means turn right.
 * This can be abstracted on the PC side. An example command-line exchange is as follows:

 ```
mobotsh> gateconfig
Turn at gate 1? (R/L)
R
Turn at gate 2? (R/L)
L
Turn at gate 3? (R/L)
R
...
Setting mobot bit vectors...
Configuration finished.
mobotsh>
```

2. Obtain battery level from ADC
 * We dedicate an ADC pin to reading the battery level
 * We define a key/value pair called "Battery Level". This is kept updated by the
 main loop at intervals. It is obtained by sending "Request Value n" packets.
 * Alternatively, we create a new packet called "Get Battery Level" that triggers
 an ADC reading and does not use the key/value table. This is an exception, not the rule
 since the ADC should not need to be used continuously.
 * On the other hand, if the key/value table is used we could output the battery level in the .csv and plot it as a function of time. This would be a really useful plot!

```
mobotsh> battery
Requesting battery level...
6.7V (85%)
mobotsh>
```

## Configuration

####XBee Config.

Channel: 0xD
PanID: 0xDEAD
Baud rate: 9600 [3]
Xbee 4414: 
  * My Address 2
  * Destination Address 1
  * NI Mobot_2
Xbee 4014 
  * My Address 1
  * Destination Address 2
  * NI Mobot_1
  
