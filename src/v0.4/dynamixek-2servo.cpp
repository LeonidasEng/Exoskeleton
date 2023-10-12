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
