# Mobot_ACE

## Mobot Control System Requirements

#### Features (by priority):

1. Wirelessly Retrive Values
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
2. CLI, with a simple set of commands defined

####Implementation Details

1. A serial (UART) line is established between the mobot (arduino) and the pc using XBee Radios
2. Messages between the PC and Mobot are packetized. Several message types are defined for the different types of data exchanges, i.e:
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
3. User Interface
  1. The PC program can write the output of a test to a .csv file. This
  would allow us to view and graph our test data to more easily tune our
  algorithms.
  2. The PC can display values on request by the user. If we see the mobot
  do something weird, we can immediately request any of the values in the
  key/value array.
  3. The PC can output values continuously, similar to our previous printf
  approach but cleaner. Using the "Configure Test Output" packet, you would
  specify which values would be displayed.
   
## Configuration

####XBee Config.

PanID: DEAD
Baud rate: 1115200 [7]
Xbee 4414: 
  * My Address 1
  * Destination Address 2
Xbee 4014 
  * My Address 2
  * Destination Address 1
  
