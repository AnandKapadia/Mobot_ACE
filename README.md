# Mobot_ACE

## Mobot Control System Requirements

#### Features (priority):

1. Wirelessly Retrive Values
  *Arduino presents a set of key-value pairs
  *Values can be requested by the PC by name/index at any time
  *Values can be streamed at intervals or when updated

2. Wirelessly Overwrite Values
  *PC can supply a new value for a key

3. Remote Controle Using Gamepad
  *2)occurs sufficiently fast and with appropriate data (ie. motorspeed and steering) such that the mobot can be remote-controlled
  *1) and 2) can continue to occur while manual control is enabled, allowing us to record data from manual tests. 

#### Interface Options - PC side

1. GUI, such as Python Tk or JAVA
2. CLI, with a siple set of commands defined

####Implementation Details

1. A serial (UART) line is established between the mobot (arduino) and the pc using XBee Radios
2. Messages between the PC and Mobot are packetized. Several message types are defined for the different types of data exchanges, i.e:
  a. "Ping"/"ID" packets.
    * cause the mobot to identify itself 
    * tests whether the mobot is present
  b. "Request Value Table"
    * Mobot responds with a list of the currently defined keys and values.
    * Key/value pairs may be stored in an array on the Mobot rather than a hash table. This would eliminiate the need to send the key strings through the network repeatedly.
  c. "Request value n"
  	* Mobot responds with the value at array index n.
  d. "Set value n to x" 
  	* Mobot sets A[n] = x.

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
  