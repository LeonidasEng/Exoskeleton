/***********************************************************************************************************************************\
|*                                                           Exogames - Project                                                    *|
|*                                                            Code created by:                                                     *|
|*                                                              LeonidasEng                                                        *|
|*                                                                                                                                 *|
|*                                              Exoskeleton - Electronic System Prototype                                          *|
|*                                                                  v0.6                                                           *|
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
* Minimum Viable Product

Motors are controlled via an identifier in DXL_IDs
- Motor 1 with ID of 1 Right Shoulder
- Motor 2 with ID of 3 Left Shoulder

The following code controls a Dynamixel Servo Motor XM430-W210-R using an Arduino Mega and Dynamixel Shield. Tension positions are
controlled by using the 3 way rotary switch. Settings include:
0 - No Tension.
1 - 1 rotation of Tension
2 - 2 rotations of Tension
These values can be adjusted dependent on exo user feedback. Next steps will be to include potentiometer/flex sensor to increase/
decrease tension based on switch position.

*/

#include <Arduino.h>
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


const uint8_t DXL_IDs[] = {1, 3}; // Dynamixel motors are ID'd in the EEPROM control table
const float DXL_PROTOCOL_VERSION = 2.0; // Standard protocol for XM430

// Bus controller object DXL
DynamixelShield dxl;

//Digital pins connected to the 3-way switch
const uint8_t switchPin1 = 22;
const uint8_t switchPin2 = 23;
const uint8_t switchPin3 = 24;

uint16_t homePositionValue = 2048; // start with a default position
int basePosition = 2048;  // for setting centers of ID 1 2048 = 0
int basePosition3 = 2048; // for setting centers of ID 3 2048 = 0
int ID1_pos2_center = homePositionValue - 4096; // One rotation in the negative direction
int ID3_pos2_center = homePositionValue + 4096; // One rotation in the positive direction
int ID1_pos3_center = homePositionValue - 2 * 4096; // Two rotations in the negative direction
int ID3_pos3_center = homePositionValue + 2 * 4096; // Two rotations in the positive direction

//This namespace is required to use Control table item i.e. velocity, baud
using namespace ControlTableItem;

// Speed Control declaration
bool setSpeed(Dynamixel2Arduino &dxl, uint8_t* DXL_IDs, float speedPct); 

void setup() {
  //Serial.begin(9600);
  // For Uno, Nano, Mini, and Mega, use UART port of DYNAMIXEL Shield to debug.
  Serial.begin(115200);
  
  // Set Port baudrate to 57600bps. This matches with DYNAMIXEL XM430 default baudrate.
  dxl.begin(57600);

  // Set Port Protocol Version. This has to match with DYNAMIXEL protocol version.
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);

  // Verifies connection between controller and Dynamixel motors.
  for (int i = 0; i < sizeof(DXL_IDs) / sizeof(DXL_IDs[0]); i++) {
    if (dxl.ping(DXL_IDs[i])) {
      Serial.println("Connection successful to Dynamixel motor with ID " + String(DXL_IDs[i]) + ".");
    } else {
      Serial.println("Failed to connect to Dynamixel motor with ID " + String(DXL_IDs[i]) + ".");
    }
  }

  // Turn off torque when configuring items in EEPROM area (saved even after power loss)
  // Iterate through each motor ID in the DXL_IDs array
  for (size_t i = 0; i < sizeof(DXL_IDs) / sizeof(DXL_IDs[0]); i++) {
      // Set torque off for the current motor ID
      dxl.torqueOff(DXL_IDs[i]);

      // Set operating mode for the current motor ID
      dxl.setOperatingMode(DXL_IDs[i], OP_POSITION);


      // Set torque on for the current motor ID
      dxl.torqueOn(DXL_IDs[i]);
  }
  Serial.println("Operating Mode changed to: Position Mode");

  // Speed call - Required every time Operating Mode is changed
  float speedPct = 50.0; // 50% speed is equivalent to 38.5 RPM (No load) at 12V
  setSpeed(dxl, DXL_IDs, speedPct); 

  // Turn off torque when configuring items in EEPROM area (saved even after power loss)
  // Iterate through each motor ID in the DXL_IDs array
  for (size_t i = 0; i < sizeof(DXL_IDs) / sizeof(DXL_IDs[0]); i++) {
      // Set torque off for the current motor ID
      dxl.torqueOff(DXL_IDs[i]);

      // Set operating mode for the current motor ID
      dxl.setOperatingMode(DXL_IDs[i], OP_EXTENDED_POSITION);
  
      // Set torque on for the current motor ID
      dxl.torqueOn(DXL_IDs[i]);
  }
  Serial.println("Operating Mode changed to: Extended Position Mode");
  
  // Speed call - Required every time Operating Mode is changed
  speedPct = 50.0;  // 50% speed is equivalent to 38.5 RPM (No load) at 12V
  setSpeed(dxl, DXL_IDs, speedPct); 

  // Enables pins for user buttons
  pinMode(switchPin1, INPUT_PULLUP);
  pinMode(switchPin2, INPUT_PULLUP);
  pinMode(switchPin3, INPUT_PULLUP);

  Serial.println("Setup Complete!");
  delay(1000);
 
}

void loop() 
{

  // Read switch positions
  int position1 = digitalRead(switchPin1);
  int position2 = digitalRead(switchPin2);
  int position3 = digitalRead(switchPin3);
  
  SerialMon();

  // Handle switch positions using a switch statement
  switch (position1 | position2 << 1 | position3  << 2) 
  {
    case 0b001: // Position 1
      basePosition = homePositionValue;
      basePosition3 = homePositionValue;
      break;

    case 0b010: // Position 2
      basePosition = ID1_pos2_center;
      basePosition3 = ID3_pos2_center;
      break;

    case 0b100: // Position 3
      basePosition = ID1_pos3_center;
      basePosition3 = ID3_pos3_center;
      break;

    default:
      // Handle any other case if needed
      break;
  }

  // Set goal positions for both motors
  dxl.setGoalPosition(DXL_IDs[0], basePosition); // Set Motor ID1 (DXL_IDs[0]) position
  dxl.setGoalPosition(DXL_IDs[1], basePosition3); // Set Motor ID3 (DXL_IDs[1]) position

}

void SerialMon()
{
  static unsigned long lastMillis = 0; // Last Time
  const unsigned long interval = 100;
  if (millis() - lastMillis >= interval)
  {
    // Move cursor to the beginning of the line with '\r'
    Serial.print("\r");
    Serial.print("Motor 1 (ID 1) Position: ");
    Serial.print(dxl.getPresentPosition(DXL_IDs[0], UNIT_DEGREE));
    Serial.print(" | Motor 2 (ID 3) Position 2: ");
    Serial.println(dxl.getPresentPosition(DXL_IDs[1], UNIT_DEGREE));

    lastMillis = millis();
  }
}

// Custom velocity function - ENTER speed percentage in SETUP()
bool setSpeed(Dynamixel2Arduino &dxl, uint8_t* DXL_IDs, float speedPct) 
{
  bool success = true;
  double maxDynamixelSpeed = 1023 * 0.229; // RPM
  uint32_t newSpeedRpm = speedPct * maxDynamixelSpeed / 100;
  uint32_t writeTimeout = 100; // ms
  
  Serial.print("\r");
  
  for (int i = 0; i < sizeof(DXL_IDs) / sizeof(DXL_IDs[0]); i++) {
    if (dxl.writeControlTableItem(PROFILE_VELOCITY, DXL_IDs[i], newSpeedRpm, writeTimeout)) {
      Serial.println("Speed successfully changed to " + String(newSpeedRpm) + "rpm for Dynamixel motor with ID " + String(DXL_IDs[i]) + ".");
    } else {
      Serial.println("Failed to set speed for Dynamixel motor with ID " + String(DXL_IDs[i]) + ".");
      success = false;
    }
  }
  return success;
}



