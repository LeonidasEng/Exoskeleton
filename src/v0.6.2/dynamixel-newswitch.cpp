/***********************************************************************************************************************************\
|*                                                           Exogames - Project                                                    *|
|*                                                            Code created by:                                                     *|
|*                                                              LeonidasEng                                                        *|
|*                                                                                                                                 *|
|*                                              Exoskeleton - Electronic System Prototype                                          *|
|*                                                                  v0.6.1                                                         *|
\***********************************************************************************************************************************/

/* 
Copyright (c) 2023, LeonidasEng
All rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the root directory of this source tree. 

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

* v0.6.2: This version now has a new rocker switch 
*/

#include <Arduino.h>
#include <Dynamixel2Arduino.h>
#include <DynamixelShield.h>

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560)
#include <SoftwareSerial.h>
SoftwareSerial soft_serial(7, 8);  // DYNAMIXELShield UART RX/TX
#define DXL_SERIAL   Serial
#define DEBUG_SERIAL soft_serial
#define DXL_DIR_PIN 2 // DYNAMIXEL Shield DIR PIN
#elif defined(ARDUINO_SAM_DUE) || defined(ARDUINO_SAM_ZERO)
#define DEBUG_SERIAL SerialUSB
#else
#define DEBUG_SERIAL Serial
#endif

const uint8_t DXL_IDs[] = {1, 3}; // Dynamixel motors are ID'd in the EEPROM control table
const float DXL_PROTOCOL_VERSION = 2.0; // Standard protocol for XM430
const uint8_t DXL_IDs_size = sizeof(DXL_IDs) / sizeof(DXL_IDs[0]);
uint8_t MAX_ATTEMPTS = 5; // Ping requests before time out.

// Bus controller object DXL
Dynamixel2Arduino dxl(DXL_SERIAL, DXL_DIR_PIN);

//Digital pin connected to the 3-way switch
const uint8_t switchPin = 22;


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
bool checkMotorConnection (uint8_t* DXL_IDs, bool* isConnected, size_t size, int MAX_ATTEMPTS = 5); 
void(* resetFunc) (void) = 0; // Reset function used in the event of time out.

void setup() 
{
  // For Uno, Nano, Mini, and Mega, use UART port of DYNAMIXEL Shield to debug.
  DEBUG_SERIAL.begin(115200);
  
  // Set Port baudrate to 57600bps. This matches with DYNAMIXEL XM430 default baudrate.
  dxl.begin(57600);

  // Set Port Protocol Version. This has to match with DYNAMIXEL protocol version.
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);

  bool isConnected[DXL_IDs_size];
  checkMotorConnection(DXL_IDs, isConnected, DXL_IDs_size);
  
  uint8_t connectedCount = 0;
  for (uint8_t i = 0; i < DXL_IDs_size; i++) {
    if (isConnected[i]) {
      connectedCount++;
    }
  }
  switch (connectedCount) {
    case 0:
      // If no motors are connected, reset the Arduino.
      DEBUG_SERIAL.println("No motors are connected. Resetting Arduino.");
      resetFunc();
      break;
    case 1:
      // If only one motor is connected, continue normal operation.
      DEBUG_SERIAL.println("Only one motor is connected. Operation will continue but 2 motors are recommended.");
      DEBUG_SERIAL.println();
      break;
    default:
      // If both motors are connected, notify of optimal setup.
      DEBUG_SERIAL.println("Both motors are connected. Optimal setup established.");
      DEBUG_SERIAL.println();
      break;
  }

  // Turn off torque when configuring items in EEPROM area (saved even after power loss)
  // Iterate through each motor ID in the DXL_IDs array
  for (uint8_t i = 0; i < DXL_IDs_size; i++) {
      // Set torque off for the current motor ID
      dxl.torqueOff(DXL_IDs[i]);

      // Set operating mode for the current motor ID
      dxl.setOperatingMode(DXL_IDs[i], OP_EXTENDED_POSITION);
  
      // Set torque on for the current motor ID
      dxl.torqueOn(DXL_IDs[i]);
  }
  DEBUG_SERIAL.println("Operating Mode changed to: Extended Position Mode");
  DEBUG_SERIAL.println();

  // Speed call - Required every time Operating Mode is changed
  float speedPct = 50.0;  // 50% speed is equivalent to 38.5 RPM (No load) at 12V
  if (!setSpeed(dxl, DXL_IDs, speedPct)) {
    DEBUG_SERIAL.println("Failed to set speed for one or more motors!");
    DEBUG_SERIAL.println();
    delay(1000);
  }

  // Enables pin for user buttons
  pinMode(switchPin, INPUT_PULLUP);
  
  DEBUG_SERIAL.println();
  DEBUG_SERIAL.println("Setup Complete!");
  delay(1000);
  DEBUG_SERIAL.println();
 
}

void loop() 
{

  // Read switch positions
  int switchState = digitalRead(switchPin);
  
//

  SerialMon();

  // Handle switch positions using a switch statement
  switch (switchState) 
  {
    case HIGH: // Position 1
      basePosition = homePositionValue;
      basePosition3 = homePositionValue;
      break;

    case LOW: // Position 2
      basePosition = ID1_pos3_center;
      basePosition3 = ID3_pos3_center;
      break;

    default: // Neutral position
      basePosition = ID1_pos2_center;
      basePosition3 = ID3_pos2_center;
      break;
  }

  // Set goal positions for both motors
  dxl.setGoalPosition(DXL_IDs[0], basePosition); // Set Motor ID1 (DXL_IDs[0]) position
  dxl.setGoalPosition(DXL_IDs[1], basePosition3); // Set Motor ID3 (DXL_IDs[1]) position

}

void SerialMon()
{
  static unsigned long lastMillis = 0; // Last Time
  const unsigned long interval = 500;
  if (millis() - lastMillis >= interval)
  {
    // Move cursor to the beginning of the line with '\r'
    DEBUG_SERIAL.print("\r");
    DEBUG_SERIAL.print("Motor 1 (ID 1) Position: ");
    DEBUG_SERIAL.print(dxl.getPresentPosition(DXL_IDs[0]));
    DEBUG_SERIAL.print(" | Motor 2 (ID 3) Position 2: ");
    DEBUG_SERIAL.println(dxl.getPresentPosition(DXL_IDs[1]));

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
  
  DEBUG_SERIAL.print("\r");
  
  for (uint8_t i = 0; i < DXL_IDs_size; i++) {
    if (dxl.writeControlTableItem(PROFILE_VELOCITY, DXL_IDs[i], newSpeedRpm, writeTimeout)) {
      DEBUG_SERIAL.println("Speed successfully changed to " + String(newSpeedRpm) + "rpm for Dynamixel motor with ID " + String(DXL_IDs[i]) + ".");
      DEBUG_SERIAL.println();
      delay(500);
    } else {
      DEBUG_SERIAL.println("Failed to set speed for Dynamixel motor with ID " + String(DXL_IDs[i]) + ".");
      DEBUG_SERIAL.println();
      success = false;
    }
  }
  return success;
}

// This function checks connection with a motor can be established, if not it will attempt it again until it reaches MAX_ATTEMPTS 
bool checkMotorConnection(uint8_t* DXL_IDs, bool* isConnected, size_t size, int MAX_ATTEMPTS = 5) 
{
  bool allConnected = true;
  for (size_t i = 0; i < size; i++) {
    int attempts = 0;
    while (!dxl.ping(DXL_IDs[i]) && attempts < MAX_ATTEMPTS) {
      DEBUG_SERIAL.println("Failed to connect to Dynamixel motor with ID " + String(DXL_IDs[i]) + ". Retrying...");
      DEBUG_SERIAL.println();
      delay(1000);  // Delay to prevent flooding requests to motor
      attempts++;
    }
    if (attempts == MAX_ATTEMPTS) {
      DEBUG_SERIAL.println("Failed to connect to Dynamixel motor with ID " + String(DXL_IDs[i]) + ". Request timed out.");
      isConnected[i] = false;
      allConnected = false;
      DEBUG_SERIAL.println();
    } else {
      DEBUG_SERIAL.println("Connection successful to Dynamixel motor with ID " + String(DXL_IDs[i]) + ".");
      isConnected[i] = true;
      DEBUG_SERIAL.println();
    }
  }
  return allConnected;  // If we got through all motors without returning false, return true
}




