/***********************************************************************************************************************************\
|*                                                           Exogames - Project                                                    *|
|*                                                            Code created by:                                                     *|
|*                                                              LeonidasEng                                                        *|
|*                                                                                                                                 *|
|*                                              Exoskeleton - Electronic System Prototype                                          *|
|*                                                                v0.3.1                                                           *|
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
