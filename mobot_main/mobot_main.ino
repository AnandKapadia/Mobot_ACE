#include <Wire.h>
#include "Adafruit_MCP23008.h"
#include <SoftwareSerial.h>
// Basic pin reading and pullup test for the mcp_front23008 I/O expander
// public domain!

// Connect pin #1 of the expander to Analog 5 (i2c clock)
// Connect pin #2 of the expander to Analog 4 (i2c data)
// Connect pins #3, 4 and 5 of the expander to ground (address selection)
// Connect pin #6 and 18 of the expander to 5V (power and reset disable)
// Connect pin #9 of the expander to ground (common ground)

// Input #0 is on pin 10 so connect a button or switch from there to ground

#define MANUAL_MODE 8

SoftwareSerial mySerial(11, 10); // RX, TX
String variableToChange = "";
String variableToRead = "";
String valueToChange = "";
String type = "";
String msg = "";
boolean changeReady = false;
boolean readReady = false;
int strStart = 0;
int strEnd = 0;

int testInt = 0;
char testChar = 'a';
String testString = "hi";
int manual_mode_state = 0;

Adafruit_MCP23008 mcp_front, mcp_back;

int back_pins[8] = {0, 1, 2, 3, 5, 6, 7, 4};
int front_pins[8] = {0, 1, 2, 3, 4, 5, 6, 7};

void setup() {  
  mcp_front.begin(1);      // use default address 0
  mcp_back.begin(0);
  Serial.begin(9600);
  mySerial.begin(9600);
  for(int i = 0; i < 8; i++)
  {
    mcp_front.pinMode(i, INPUT);
    mcp_back.pinMode(i, INPUT);
    mcp_front.pullUp(i, HIGH);  // turn on a 100K pullup internally
    mcp_back.pullUp(i, HIGH);  // turn on a 100K pullup internally
  }
  pinMode(MANUAL_MODE, OUTPUT);
  pinMode(13, OUTPUT);  // use the p13 LED as debugging
  mySerial.println("Serial Communications Online");
}

void loop() {
  if (mySerial.available()) { // only data of form ((write))variable==value++ or 
    handleSerial();           // ((read))variable++ will be read by this function
  }
  if (changeReady) {
    handleChange();
    changeReady = false;
  }
  if (readReady) {
    handleRead();
    readReady = false;
  }
  for(int i = 0; i < 8; i++)
  {
    Serial.print(mcp_front.digitalRead(front_pins[i]));
    Serial.print(" ");
  }  
  for(int i = 0; i < 8; i++)
  {
    Serial.print(mcp_back.digitalRead(back_pins[i]));
    Serial.print(" ");
  }
  Serial.println();
  // The LED will 'echo' the button 
}

void printSensorData()
{
  for(int i = 0; i < 8; i++)
  {
    mySerial.print(mcp_front.digitalRead(front_pins[i]));
    mySerial.print(" ");
  }  
  for(int i = 0; i < 8; i++)
  {
    mySerial.print(mcp_back.digitalRead(back_pins[i]));
    mySerial.print(" ");
  }
  mySerial.println();
}


void handleRead() {
  if (variableToRead.equals("testInt")) {
    mySerial.print("The variable testInt has value: ");
    mySerial.println(testInt);
  }
  else if (variableToRead.equals("testChar")) {
    mySerial.print("The variable testChar has value: ");
    mySerial.println(testChar);
  }
  else if (variableToRead.equals("testString")) {
    mySerial.print("The variable testString has value: ");
    mySerial.println(testString);
  }
  else if (variableToRead.equals("printSensorData")) {
    printSensorData();
  }
  else if (variableToRead.equals("manual_mode_state")) {
    mySerial.print("The variable manual_mode_state has value: ");
    mySerial.println(manual_mode_state);
  }
  else {
      mySerial.println("Not a valid or handled variable");
  }
}

void handleChange() {
  if (variableToChange.equals("testInt")) {
    testInt = valueToChange.toInt();
  }
  else if (variableToChange.equals("testChar")) {
    testChar = (char)valueToChange[0];
  }
  else if (variableToChange.equals("testString")) {
    testString = valueToChange;
  }
  else if (variableToChange.equals("manual_mode_state")) {
    digitalWrite(MANUAL_MODE, valueToChange.toInt());
    manual_mode_state = valueToChange.toInt();
  }
  else {
      mySerial.println("Not a valid or handled variable");
      return;
  }
  variableToRead = variableToChange;
  handleRead();
}

void handleSerial() {
   msg = "";
   if(mySerial.available())
     msg = mySerial.readString();
   strStart = msg.indexOf("((");
   strEnd   = msg.indexOf("))");
   if (strStart < 0 || strEnd < 0) {
     mySerial.println("invalid format");
     return;
   }
   strEnd += 2;
   type = msg.substring(strStart, strEnd);
   
   if (type.equals("((write))")) {
     strStart = strEnd;
     strEnd = msg.indexOf("==");
     if (strEnd < 0) {
       mySerial.println("invalid format");
       return;
     }
     variableToChange = msg.substring(strStart, strEnd);
     strStart = strEnd + 2;
     strEnd = msg.indexOf("++");
     if (strEnd < 0) {
       mySerial.println("invalid format");
       return;
     }
     valueToChange = msg.substring(strStart, strEnd);
     changeReady = true;
     return;
   }
   
   else if (type.equals("((read))")) {
     strStart = strEnd;
     strEnd = msg.indexOf("++");
     if (strEnd < 0) {
       mySerial.println("invalid format");
       return;
     }
     variableToRead = msg.substring(strStart, strEnd);
     readReady = true;
     return;
   }
   
   else {
     mySerial.println("invalid format");
   }
}
