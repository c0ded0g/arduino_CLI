/*
  CLI DECODER.
  
  Monitor the phone line and decode CLI information on incoming calls.
  Send decoded CLI information to RPi for subsequent handling.
  
  
*/

// PIN ASSIGNMENTS
const int pin_A0 = A0;
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
const int pin_11 = 11;
const int pin_12 = 12;
const int pin_13 = 13;

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
  a = a+1;
  sprintf(dataString,"%02X",a);
  Serial.println(dataString);
  digitalWrite(led,HIGH);
  delay(50);
  digitalWrite(led,LOW);
  delay(950);
}


