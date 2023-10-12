/***********************************************************************************************************************************\
|*                                                           Exogames - Project                                                    *|
|*                                                            Code created by:                                                     *|
|*                                                              LeonidasEng                                                        *|
|*                                                                                                                                 *|
|*                                              Exoskeleton - Electronic System Prototype                                          *|
|*                                                                 v0.3                                                            *|
\***********************************************************************************************************************************/

/* 
Exo Games 2023
Copyright (C) 2023  LeonidasEng

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

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

/* Continuous Servo Motor - 3 positions */
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
