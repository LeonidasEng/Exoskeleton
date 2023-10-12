/***********************************************************************************************************************************\
|*                                                           Exogames - Project                                                    *|
|*                                                            Code created by:                                                     *|
|*                                                              LeonidasEng                                                        *|
|*                                                                                                                                 *|
|*                                              Exoskeleton - Electronic System Prototype                                          *|
|*                                                                  v0.4                                                           *|
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

This version is used to explore how the dynamixel motors would work with a flex sensor 

*/

#include <Arduino.h>
#include <Dynamixel2Arduino.h>
#include <movingAvg.h>

#define DXL_ID 1 // DYNAMIXEL ID
#define BAUDRATE 57600 // Baud rate of DYNAMIXEL
#define OPERATING_MODE OP_POSITION // Operating mode

movingAvg avgpos(30);
int flexSensorPin = A0; // Analog pin used to connect the flex sensor
int val; // Variable to read the value from the analog pin
int avgFlexSensorVal; // Stores the moving average of the flex sensor values
int pos; // Represents the current position of the servo

Dynamixel2Arduino dxl;

void setup() {
  Serial.begin(9600);
  dxl.begin(BAUDRATE);
  dxl.ping(DXL_ID);
  dxl.setOperatingMode(DXL_ID, OPERATING_MODE);
  avgpos.begin();
}

void loop() {
  val = analogRead(flexSensorPin); // Reads the value of the flex sensor (value between 0 and 1023)
  avgFlexSensorVal = avgpos.reading(val);

  // DYNAMIXEL XM430-W210-R Position - 3 positions
  if (avgFlexSensorVal < pos){
    val = dxl.convertRadian2Value(DXL_ID, 0); // Minimum position
  } else if (avgFlexSensorVal == pos){
    val = dxl.convertRadian2Value(DXL_ID, 180*DXL2Arduino::DEG2RAD/2); // Middle position
  } else if (avgFlexSensorVal > pos){
    val = dxl.convertRadian2Value(DXL_ID, 180*DXL2Arduino::DEG2RAD); // Maximum position
  }

  pos = avgFlexSensorVal;

  dxl.setGoalPosition(DXL_ID, val); // Sets the servo position according to the scaled value

  Serial.print("Moving Avg: ");
  Serial.println(avgFlexSensorVal);

  delay(30); // Waits for the servo to get there
}
