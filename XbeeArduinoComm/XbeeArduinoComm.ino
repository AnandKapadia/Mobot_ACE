
#include <SoftwareSerial.h>

SoftwareSerial mySerial(8, 7); // RX, TX
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

void setup()  
{
  Serial.begin(9600);
  mySerial.begin(9600);
}

void loop() 
{
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



