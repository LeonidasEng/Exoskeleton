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
*/
#include <DynamixelShield.h>

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560)
  #define SERIAL_PORT Serial
#else
  #define SERIAL_PORT SerialUSB
#endif

#define DIR_PIN 2  //Direction pin for Dynamixel Shield
#define OPERATING_MODE OP_POSITION // Operating mode

DynamixelShield dxl; // Bus control for ALL attached servos

//Digital pins connected to the 3-way switch
const int switchPin1 = 2;
const int switchPin2 = 3;
const int switchPin3 = 4;

// Global speed variable
const float globalSpeed = 100.0; // Set the speed as needed

void setup() {
  SERIAL_PORT.begin(57600);
  dxl.begin(SERIAL_PORT, DIR_PIN);
  
  dxl.setPortProtocolVersion(2.0);
  dxl.torqueOn(1);  // Turn on torque for servo with ID 1
  dxl.torqueOn(2);  // Turn on torque for servo with ID 2

  // Set the global speed for the servos
  dxl.setGoalVelocity(1, globalSpeed);
  dxl.setGoalVelocity(2, globalSpeed);

  pinMode(switchPin1, INPUT);
  pinMode(switchPin2, INPUT);
  pinMode(switchPin3, INPUT);
}

void loop() {
  // Read switch position
  int position1 = digitalRead(switchPin1);
  int position2 = digitalRead(switchPin2);
  int position3 = digitalRead(switchPin3);

  // Position 1
  if (position1 == HIGH && position2 == LOW && position3 == LOW) {
    dxl.setGoalPosition(1, 4096 * 2);  // two rotations for servo 1
    dxl.setGoalPosition(2, -4096 * 2);  // two rotations for servo 2 in opposite direction
  }
  // Position 2
  else if (position1 == LOW && position2 == HIGH && position3 == LOW) {
    dxl.setGoalPosition(1, 4096);  // one rotation for servo 1
    dxl.setGoalPosition(2, -4096);  // one rotation for servo 2 in opposite direction
  }
  // Position 3
  else if (position1 == LOW && position2 == LOW && position3 == HIGH) {
    dx1.setGoalVelocity(1, 10);
    dx1.setGoalVelocity(2, 10);
    dxl.setGoalPosition(1, 0);  // return to position zero for servo 1
    dxl.setGoalPosition(2, 0);  // return to position zero for servo 2
  }

  delay(100);  // delay to prevent bouncing
}
