/*
  CLI DECODER.
  
  Monitor the phone line and decode CLI information on incoming calls.
  Send decoded CLI information to RPi for subsequent handling.
  
  to be completed
  
*/

// PIN ASSIGNMENTS
// Analog Pins as an array, so they can be accessed in a loop
static const uint8_t AnPin[] = {A0, A1, A2, A3, A4, A5};
static const String AnPinName[] = {"A0", "A1", "A2", "A3", "A4", "A5"};
static const String AnValName[] = {"V0", "V1", "V2", "V3", "V4", "V5"};


const int pin_CLI = A1;    // HT9032 data pin (Vout)
const int pin_A2 = A2;
const int pin_RING = A3;   // HT9032/ring sense
const int pin_PDWN = A4;   // HT9032/PDWN
const int pin_A5 = A5;

const int pin_0 = 0;
const int pin_1 = 1;
const int pin_2 = 2;
const int pin_3 = 3;
const int pin_4 = 4;
const int pin_5 = 5;
const int pin_6 = 6;
const int pin_7 = 7;
const int pin_8 = 8;
const int pin_9 = 9;
const int pin_10 = 10;
const int pin_LED = 11;    // visual indicator incoming call
const int pin_12 = 12;
const int pin_13 = 13;

// array containing analog pin values
int AnVal[] = {0,0,0,0,0,0};  // A0..A5
int outVal[] = {0,0,0,0,0,0}; // mapped equivalents

int outputValue = 0;        // value output to the PWM (analog out)

// INCLUDES
#include <TimerOne.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(A1,A2);

// CONSTANTS
const int timeOut = 1000;
const int maxCIDs = 20;

// VARS
int t_now;
int t_prev;
int CIDpointer = 0;
int CIDpointerPrev = -1;
int nextCIDindex = 0;

struct callerInfo {
  String callerID;
  String timestamp;
  String month;
  byte day;
  byte hour;
  byte minute;
};

callerInfo callList[maxCIDs];

// TEST CODE (to be deleted)
char dataString[50] = {0};
int a=0;
int led=13;

void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(pin_PDWN, OUTPUT);
  // set the data rate for serial comms to HT9032
  mySerial.begin(1200);
}

void loop() {
  // select only one loop ...
  testLoop1();
  //testLoop2();
  //CLIloop();
}

void testLoop1() {
  // monitor an analog input to vary the brightness of an LED
  // send parameters to the serial port in JSON format (single line) for the RPi to read

  // read all analog pins  
  // note AN0 is connected to a pot
  for (int i=0; i<=5; i++) {
    AnVal[i] = analogRead(AnPin[i]);
  }
  
  // continually read An pins for a second
  for (int i=0; i <=20; i++) {
    for (int j=0; j<=5; j++) {
      AnVal[j] = analogRead(AnPin[j]);
      outVal[j] = map(AnVal[j], 0, 1023, 0, 255);
      if (outVal[1] > 128) {
        analogWrite(pin_9, outVal[0]);
      } else {
        analogWrite(pin_9, 0);
      }
      //outputValue = map(AnVal[1], 0, 1023, 0, 255);  
      //analogWrite(pin_9, outputValue);           
      delay(10);
    }
  }
  
  // once a second send the values to the serial port, in JSON form
  for (int i=0; i<=5; i++) {
    Serial.println(formJSONpair(AnPinName[i],String(AnVal[i])));
    Serial.println(formJSONpair(AnValName[i],String(outVal[i])));
  }  
//  Serial.println(formJSONpair("V1",String(outputValue)));
}

String formJSONpair(String n, String v) {
  // given n and v, return {"n":v}
  String s = String("{\""+n+"\":"+v+"}");
  return s;
}
String addJSONpair(String j, String n, String v) {
  // given j (of the form {...}), n and v, return {..., "n":v}
  // i.e. add another pair to an existing JSON object
  String s = j;
  int length = s.length();
  s[length-1] = ',';
  s = s+" \""+n+"\":"+v+"}\0";
  return s;
}

void testLoop2() {
}

void CLIloop() {
  char ringing;
  char serialChar;
  char ch;
  int countSieze;
  int messageType;
  int messageLength;
  int paramLength;
  int byteCounter;
  int paramType;
  boolean waiting;
  boolean channelSiezeDetected;
  String s;
  int i;
  
//  a = a+1;
//  Serial.println(a);
//  digitalWrite(led,HIGH);
//  delay(50);
//  digitalWrite(led,LOW);
//  delay(950);
  
  
  
  
  ringing = digitalRead(A3);
  if (ringing == 1) {
    // enable the HT9032 and look for the channel seize bit pattern
    digitalWrite(pin_LED, HIGH);
    digitalWrite(A4, LOW);
    t_now = millis();
    t_prev = t_now;
    countSieze = 0;
    waiting = true;
    channelSiezeDetected = false;
    // repeat this loop until either the channel seizure is detected
    // or a timeout occurs
    do {
      
      if (mySerial.available()) {
        serialChar = mySerial.read();
        if (serialChar == 'U') {
          countSieze += 1;
        }
      }
      
      if (countSieze == 30) {
        waiting = false;
        channelSiezeDetected = true;
      }
      
      t_now = millis();
      if ((t_now - t_prev) > 1600) {
        waiting = false;
      }
      
    } while (waiting);
    
    if (channelSiezeDetected) {
      messageLength = 0;
      // get first 2 bytes (message type, message length)
      while (!mySerial.available()) {}
      messageType = mySerial.read();
      while (!mySerial.available()) {}
      messageLength = mySerial.read();
      // get remaining bytes
      byteCounter = 0;
      waiting = true;
      do {
        // get parameter type & parameter length
        while (!mySerial.available()) {}
        paramType = mySerial.read();
        byteCounter += 1;
        while (!mySerial.available()) {}
        paramLength = mySerial.read();
        byteCounter += 1;
           
        // get parameter value/data
        for (int j = 1; j <= paramLength; j++) {
          while (!mySerial.available()) {}
          ch = mySerial.read();
          switch (paramType) {
            case 1:
              callList[nextCIDindex].timestamp += ch;
              break;
            case 2:
              callList[nextCIDindex].callerID += ch;
              break;
            case 4:
              switch (ch) {
                case 'P':
                  callList[nextCIDindex].callerID = "withheld";
                  break;
                case 'O':
                  callList[nextCIDindex].callerID = "unavailable";
                  break;
              }
              break;
          }
          byteCounter += 1;
        }
        if (byteCounter >= messageLength) {
          waiting = false;
        }
      } while (waiting);
      
      // send caller ID to RPi
      // TBC
      Serial.println(callList[nextCIDindex].callerID);
      
    }
    // end channelSiezeDetected
      
  }
  // end ringing
  
  
  
}


