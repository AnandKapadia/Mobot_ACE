#include <Wire.h>
#include "Adafruit_MCP23008.h"
#include <SoftwareSerial.h>
#include <Servo.h>
// Basic pin reading and pullup test for the mcp_front23008 I/O expander
// public domain!

// Connect pin #1 of the expander to Analog 5 (i2c clock)
// Connect pin #2 of the expander to Analog 4 (i2c data)
// Connect pins #3, 4 and 5 of the expander to ground (address selection)
// Connect pin #6 and 18 of the expander to 5V (power and reset disable)
// Connect pin #9 of the expander to ground (common ground)

// Input #0 is on pin 10 so connect a button or switch from there to ground

#define STOPLED A0
#define STARTLED A1
#define STOP A2
#define START A3
#define STEERINGPIN 4
#define THROTTLEPIN 7
#define MANUAL_MODE 8


Servo STEERING;
Servo THROTTLE;

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
int count = 0;
int steeringFactor = 10;
int STARTDELAY = 5000;
int testInt = 0;
char testChar = 'a';
String testString = "hi";
int manual_mode_state = 0;
int throttle = 0;
int steering = 0;
int start_state = 0;
float prevFrontVal = 0;
float prevBackVal = 0;
float alpha = 10;
float beta = 5;
float frontVal = 0.0;
float backVal = 0.0;
  
Adafruit_MCP23008 mcp_front, mcp_back;

int back_pins[8] = {0,1,2,3,5,6,7,4};//{4, 7, 6, 5, 3, 2, 1, 0};
int front_pins[8] = {7, 6, 5, 4, 3, 2, 1, 0};

int front_values[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int back_values[8]  = {0, 0, 0, 0, 0, 0, 0, 0};

void setup() {  
  initialize();
  setDefaults();
  //mySerial.println("Serial Communications Online");
}

void initialize()
{
  mcp_front.begin(1);      // use default address 0
  mcp_back.begin(0);
  //Serial.begin(9600);
  mySerial.begin(9600);
  for(int i = 0; i < 8; i++)
  {
    mcp_front.pinMode(i, INPUT);
    mcp_back.pinMode(i, INPUT);
    mcp_front.pullUp(i, HIGH);  // turn on a 100K pullup internally
    mcp_back.pullUp(i, HIGH);  // turn on a 100K pullup internally
  }
  pinMode(MANUAL_MODE, OUTPUT);
  pinMode(STOP, INPUT);
  pinMode(START, INPUT);
  pinMode(STOPLED, OUTPUT);
  pinMode(STARTLED, OUTPUT);
  STEERING.attach(STEERINGPIN, 1000, 2000);
  THROTTLE.attach(THROTTLEPIN, 520, 2370);
  pinMode(13, OUTPUT);  // use the p13 LED as debugging
}

void setDefaults()
{
  start_state = 0;
  digitalWrite(STARTLED, LOW);
  digitalWrite(STOPLED, HIGH);
  digitalWrite(MANUAL_MODE, 1);
  manual_mode_state = 1;
  steering = 90;
  throttle = 90;
  STEERING.write(90);
  THROTTLE.write(90);
  
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
  handleStartState();
  getSensorValues();
  //autonomous();
  lineFollowing();
  count++;
  // The LED will 'echo' the button 
}

void autonomous()
{
  /*
  Serial.print(manual_mode_state);
  Serial.print("  ");
  Serial.print(start_state);
  Serial.print("  ");
  */
  if(start_state == 1)
  {
    steering = count%180;
    
  }
  else{
    steering = 90;
  }
  //Serial.println(steering);
  STEERING.write(180-steering);
  THROTTLE.write(180-steering);
  //delay(100);
}

void lineFollowing()
{
  if(start_state == 1)
  {
    getLineFollowingValues();
    if(steering > 180) steering = 180;
    if(throttle > 180) throttle = 180;
    if(steering < 0) steering = 0;
    if(throttle < 0) throttle = 0;
    STEERING.write(steering);
    THROTTLE.write(throttle);
  }
  else
  {
    //Serial.println("stopped state");
    STEERING.write(90);
    THROTTLE.write(90);
  }
}


float getVal(int array[])
{
  float val = 0;
  float numVal = 0;
  for(int i = 0; i <= 7; i++) {
    if (array[i] == 1) {
      val = val + (float) i;
      numVal = numVal + (float) 1;
    }
  }
  
  if (numVal == 0) {
    return -1;
  }
  else {
    return (val / numVal) - 3.5;
  }
}
  

void getLineFollowingValues()
{

  
  //this method should set steering and throttle values
  frontVal = getVal(front_values);
  backVal = getVal(back_values);
  
  if(frontVal == -1) frontVal = prevFrontVal;
  else prevFrontVal = frontVal;
  if(backVal == -1) backVal = prevBackVal;
  else prevBackVal = backVal;
  
  //Alpha should be larger than beta
  steering = int (90 - (frontVal * alpha) - ((frontVal - backVal) * beta));
  
}

void getSensorValues()
{
  uint8_t frontByte = mcp_front.readGPIO();

  for(int i = 0; i < 8; i++)
  {
    front_values[i] = !(0x01 & (frontByte >> front_pins[i]));
  }  

  uint8_t rearByte = mcp_back.readGPIO();

  for(int i = 0; i < 8; i++)
  {
    back_values[i] = !(0x01 & (rearByte >> back_pins[i]));
  }
}

void handleStartState() 
{
  if(digitalRead(STOP) == 0){
    start_state = 0;
    digitalWrite(STARTLED, LOW);
    digitalWrite(STOPLED, HIGH);
  }
  if(digitalRead(START) == 0){
    digitalWrite(STARTLED, HIGH);
    digitalWrite(STOPLED, LOW);
    delay(STARTDELAY);
    start_state = 1;
  }
}

void printSensorData()
{
  for(int i = 0; i < 8; i++)
  {
    mySerial.print(front_values[i]);
    mySerial.print(" ");
  }  
  for(int i = 0; i < 8; i++)
  {
    mySerial.print(back_values[i]);
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
  else if (variableToRead.equals("start_state")) {
    mySerial.print("The variable start_state has value: ");
    mySerial.println(start_state);
  }
  else if (variableToRead.equals("manual_mode_state")) {
    mySerial.print("The variable manual_mode_state has value: ");
    mySerial.println(manual_mode_state);
  }  
  else if (variableToRead.equals("steering")) {
    mySerial.print("The variable steering has value: ");
    mySerial.println(steering);
  }  
  else if (variableToRead.equals("throttle")) {
    mySerial.print("The variable throttle has value: ");
    mySerial.println(throttle);
  }
  else if (variableToRead.equals("steeringFactor")) {
    mySerial.print("The variable throttle has value: ");
    mySerial.println(steeringFactor);
  }
  else if (variableToRead.equals("alpha")) {
    mySerial.print("The variable alpha has value: ");
    mySerial.println(alpha);
  }
  else if (variableToRead.equals("beta")) {
    mySerial.print("The variable beta has value: ");
    mySerial.println(beta);
  }  
  else if (variableToRead.equals("frontVal")) {
    mySerial.print("The variable frontVal has value: ");
    mySerial.println(frontVal);
  }
  else if (variableToRead.equals("backVal")) {
    mySerial.print("The variable backVal has value: ");
    mySerial.println(backVal);
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
  else if (variableToChange.equals("STOP")) {
    start_state = 0;
    digitalWrite(STARTLED, LOW);
    digitalWrite(STOPLED, HIGH);
    variableToChange = "start_state";
  }
  else if (variableToChange.equals("START")) {
    start_state = 1;
    digitalWrite(STARTLED, HIGH);
    digitalWrite(STOPLED, LOW);
    variableToChange = "start_state";
  }
  else if (variableToChange.equals("steering")) {
    steering = valueToChange.toInt();
  }  
  else if (variableToChange.equals("throttle")) {
    throttle = valueToChange.toInt();
  }
  else if (variableToChange.equals("steeringFactor")) {
    steeringFactor = valueToChange.toInt();
  }
  else if (variableToChange.equals("alpha")) {
    alpha = valueToChange.toInt();
  }
  else if (variableToChange.equals("beta")) {
    beta = valueToChange.toInt();
  }
  else if (variableToChange.equals("backVal")) {
    backVal = valueToChange.toInt();
  }
  else if (variableToChange.equals("frontVal")) {
    frontVal = valueToChange.toInt();
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
