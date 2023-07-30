/***********************************************************************************************************************************\
|*                                                           Exogames - Project                                                    *|
|*                                                            Code created by:                                                     *|
|*                                                              LeonidasEng                                                        *|
|*                                                                                                                                 *|
|*                                              Exoskeleton - Electronic System Prototype                                          *|
|*                                                                  v0.4                                                           *|
\***********************************************************************************************************************************/

/* 
Copyright (c) 2023, LeonidasEng
All rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the root directory of this source tree. 

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
