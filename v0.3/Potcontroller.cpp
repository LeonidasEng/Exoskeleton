/***********************************************************************************************************************************\
|*                                                           Exogames - Project                                                    *|
|*                                                            Code created by:                                                     *|
|*                                                              LeonidasEng                                                        *|
|*                                                                                                                                 *|
|*                                              Exoskeleton - Electronic System Prototype                                          *|
|*                                                                 v0.3                                                            *|
\***********************************************************************************************************************************/

/* 
Copyright (c) 2023, LeonidasEng
All rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the root directory of this source tree. 

***********************************************************************************************************************************

Explanation of System Design:
The following code controls a continuous servo motor with a potentiometer. Difficulties with high current draw, portability and position
feedback drove this change. The continuous servo is placed on the back of the exoskeleton user and is connected to a potentiometer input.
When the potentiometer moves the servo follows it.

*/

#include <Arduino.h>
#include <Servo.h>
#include <movingAvg.h> 

Servo myservo;  // create servo object to control a servo
movingAvg avgpos(30); 
int potpin = A0; // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin
int valp;    // variable to read the value from the analog pin
int pos; // represents the current position of the servo.
int avgPot; // stores the moving average of the potentiometer values.

void setup() {
  Serial.begin(9600);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  avgpos.begin();
}

void loop() 
{

valp = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
avgPot=avgpos.reading(valp);

if (avgPot<pos){
  val=0;
}
if ((avgPot)==(pos)){
  val=90;
}
if (avgPot>pos){
  val=180;
}

pos=avgPot;

myservo.write(val);                  // sets the servo position according to the scaled value

Serial.print("Moving Avg: ");
Serial.println(avgPot);

//Serial.print("Servo Value: ");
//Serial.println(val);

delay(30);                           // waits for the servo to get there
}
