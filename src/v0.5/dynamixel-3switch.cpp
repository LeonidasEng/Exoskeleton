/***********************************************************************************************************************************\
|*                                                           Exogames - Project                                                    *|
|*                                                            Code created by:                                                     *|
|*                                                              LeonidasEng                                                        *|
|*                                                                                                                                 *|
|*                                              Exoskeleton - Electronic System Prototype                                          *|
|*                                                                  v0.5                                                           *|
\***********************************************************************************************************************************/

/* 
Copyright (c) 2023, LeonidasEng
All rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the root directory of this source tree. 

***********************************************************************************************************************************

Explanation of System Design:
* Minimum Viable Product


DXL_1 = Motor 1 with ID of 1 Right Shoulder
DXL_3 = Motor 2 with ID of 3 Left Shoulder
The following code controls a Dynamixel Servo Motor MX430-W210-R using an Arduino Mega and Dynamixel Shield. Tension positions are
controlled by using the 3 way rotary switch (using push buttons temporarily but principle not expected to change). Settings include:
0 - No Tension.
1 - 1 rotation of Tension
2 - 2 rotations of Tension
These values can be adjusted dependent on exo user feedback. Next steps will be to include potentiometer/flex sensor to increase/
decrease tension based on switch position.

*/

#include <Dynamixel2Arduino.h>
#include <DynamixelShield.h>

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560)
#include <SoftwareSerial.h>
SoftwareSerial soft_serial(7, 8);  // DYNAMIXELShield UART RX/TX
#define DEBUG_SERIAL soft_serial
#elif defined(ARDUINO_SAM_DUE) || defined(ARDUINO_SAM_ZERO)
#define DEBUG_SERIAL SerialUSB
#else
#define DEBUG_SERIAL Serial
#endif

const uint8_t DXL_1 = 1;
const float DXL_PROTOCOL_VERSION = 2.0;
uint16_t homePositionValue = 2048;

DynamixelShield dxl;

//Digital pins connected to the 3-way switch
const int switchPin1 = 22;
const int switchPin2 = 23;
const int switchPin3 = 24;


static int basePosition = 2048;  // start with a default position
int pos2_center = basePosition - 4096; // One rotation in the negative direction
int pos3_center = basePosition - 2 * 4096; // Two rotations in the negative direction


//This namespace is required to use Control table item names
using namespace ControlTableItem;

// Custom velocity function - ENTER speed percentage in SETUP()
boolean setSpeed(Dynamixel2Arduino &dxl, uint8_t DXL_3, float speedPct) 
{
    double maxDynamixelSpeed = 1023 * 0.229; //RPM
    uint32_t newSpeedRpm = speedPct * maxDynamixelSpeed / 100;
    uint32_t writeTimeout = 100; // ms
    return dxl.writeControlTableItem(PROFILE_VELOCITY, DXL_3, newSpeedRpm, writeTimeout);
}

void setup() {
  // For Uno, Nano, Mini, and Mega, use UART port of DYNAMIXEL Shield to debug.
  DEBUG_SERIAL.begin(115200);

  // Set Port baudrate to 57600bps. This has to match with DYNAMIXEL baudrate.
  dxl.begin(57600);

  // Set Port Protocol Version. This has to match with DYNAMIXEL protocol version.
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);

  // Get DYNAMIXEL information
  dxl.ping(DXL_3);

  // Turn off torque when configuring items in EEPROM area (saved even after power loss)
  dxl.torqueOff(DXL_3);
  dxl.setOperatingMode(DXL_3, OP_POSITION);
  dxl.torqueOn(DXL_3);
  //*********************************************************************************
  // Speed call - Required every time Operating Mode is changed
  float speedPct = 50.0; // for example
  if (!setSpeed(dxl, DXL_3, speedPct)) 
  {
    Serial.println("Failed to set speed");
  }
  int present_position = dxl.getPresentPosition(DXL_3);
    // if (presentposition < 2048) {
    //   dxl.setGoalPosition(DXL_3, 2048);
    //   delay(100); // Depending on how fast you want to move the motor
    // }
    if (present_position != 2048)
    {  
      dxl.setGoalPosition(DXL_3, 2048);
    }
  //*********************************************************************************
  delay(1000);
  Serial.println("Calibration Complete!");
  dxl.torqueOff(DXL_3);
  dxl.setOperatingMode(DXL_3, OP_EXTENDED_POSITION);
  dxl.torqueOn(DXL_3);

  // Speed call
  speedPct = 50.0; // for example
  if (!setSpeed(dxl, DXL_3, speedPct)) 
  {
    Serial.println("Failed to set speed");
  }

  // Enables pins for user buttons
  pinMode(switchPin1, INPUT_PULLUP);
  pinMode(switchPin2, INPUT_PULLUP);
  pinMode(switchPin3, INPUT_PULLUP);
  
}

void loop() {

  float speedPct = 50.0; // for example
  if (!setSpeed(dxl, DXL_3, speedPct)) 
  {
    Serial.println("Failed to set speed");
  }

  int position1 = digitalRead(switchPin1);
  int position2 = digitalRead(switchPin2);
  int position3 = digitalRead(switchPin3);

  // Position 1
  if (position1 == HIGH && position2 == LOW && position3 == LOW) {
    basePosition = 2048;
  
  }
  // Position 2
  else if (position1 == LOW && position2 == HIGH && position3 == LOW) {
    basePosition = pos2_center;
    
  }
  // Position 3
  else if (position1 == LOW && position2 == LOW && position3 == HIGH) {
    basePosition = pos3_center;
  }
  dxl.setGoalPosition(DXL_3, basePosition);  // set to base position
  //Serial.println("0");
  delay(100);  // delay to prevent bouncing

}
