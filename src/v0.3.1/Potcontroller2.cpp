/***********************************************************************************************************************************\
|*                                                           Exogames - Project                                                    *|
|*                                                            Code created by:                                                     *|
|*                                                              LeonidasEng                                                        *|
|*                                                                                                                                 *|
|*                                              Exoskeleton - Electronic System Prototype                                          *|
|*                                                                v0.3.1                                                           *|
\***********************************************************************************************************************************/

/* 
Copyright (c) 2023, LeonidasEng
All rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the root directory of this source tree. 

***********************************************************************************************************************************

Explanation of System Design:
The following code controls a continuous servo motor with a potentiometer. However, with this version the positions of the servo motor 
will SET tensions based on position of the arm. Keeping the arm extended will not move the motor. Moving between extension to 90 
degrees increases tension, full contraction at 145 degrees releases the tension in the arm. Testing with the physical system will
inform the required ranges for user comfort.


*/

#include <Arduino.h>
#include <Servo.h>
#include <movingAvg.h> 

Servo myservo;
movingAvg avgpos(30);
int potpin = A0;
int val;    
int pos = 0; 

void setup() {
  Serial.begin(9600);
  myservo.attach(9);  
  avgpos.begin();
}

void loop() 
{
  int rawVal = analogRead(potpin);
  int avgVal = avgpos.reading(rawVal);

  // Adjusting mapping function
  if(avgVal <= 512) {
    val = map(avgVal, 0, 512, 90, 0); // maps from 0 to 512 to 90 to 0
  } 
  else {
    val = map(avgVal, 513, 1023, 90, 180); // maps from 513 to 1023 to 90 to 180
  }

  myservo.write(val);  
  Serial.println(val);

  delay(15);  
}
