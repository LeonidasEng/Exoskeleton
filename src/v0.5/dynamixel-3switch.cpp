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

const uint8_t DXL_IDs[] = {1, 3};
const float DXL_PROTOCOL_VERSION = 2.0;
uint16_t homePositionValue = 2048;

DynamixelShield dxl;

//Digital pins connected to the 3-way switch
const int switchPin1 = 22;
const int switchPin2 = 23;
const int switchPin3 = 24;

static int basePosition = 2048;  // start with a default position
int pos2_center = 6143;
int pos3_center = 10238;

//This namespace is required to use Control table item names
using namespace ControlTableItem;

// Speed Function declaration
bool setSpeed(Dynamixel2Arduino &dxl, const uint8_t DXL_IDs, float speedPct);
// Calibration Function declaration
bool moveToZero(Dynamixel2Arduino &dxl, const uint8_t DXL_IDs);

bool moveToPosition(Dynamixel2Arduino &dxl, const uint8_t DXL_IDs, int goalPosition);

void setup() {
  // For Uno, Nano, Mini, and Mega, use UART port of DYNAMIXEL Shield to debug.
  DEBUG_SERIAL.begin(115200);

  // Set Port baudrate to 57600bps. This has to match with DYNAMIXEL baudrate.
  dxl.begin(57600);

  // Set Port Protocol Version. This has to match with DYNAMIXEL protocol version.
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);

  // Get DYNAMIXEL information
  dxl.ping(DXL_IDs[0]);
  dxl.ping(DXL_IDs[1]);

  // Turn off torque when configuring items in EEPROM area (saved even after power loss)
  // Iterate through each motor ID in the DXL_IDs array
  for (size_t i = 0; i < sizeof(DXL_IDs) / sizeof(DXL_IDs[0]); i++) {
      // Set torque off for the current motor ID
      setTorqueOff(dxl, DXL_IDs[i]);

      // Set operating mode for the current motor ID
      setOperatingMode(dxl, DXL_IDs[i], OP_POSITION);

      // Set torque on for the current motor ID
      setTorqueOn(dxl, DXL_IDs[i]);
  }
  //*********************************************************************************
  // Speed call - Required every time Operating Mode is changed
  float speedPct = 50.0; // for example
  for (int i = 0; i < sizeof(DXL_IDs); i++) {
    if (!setSpeed(dxl, DXL_IDs[i], speedPct)) {
        Serial.println("Failed to set speed");
    }
  }
  // Check if the motor ID is valid
  // Check each motor in the DXL_IDs array and move to zero position
  for (size_t i = 0; i < sizeof(DXL_IDs) / sizeof(DXL_IDs[0]); i++) {
      if (moveToZero(dxl, DXL_IDs[i])) {
          Serial.println("Motor ID " + String(DXL_IDs[i]) + " moved to zero position.");
      } else {
          Serial.println("Failed to move Motor ID " + String(DXL_IDs[i]) + " to zero position.");
      }
  }

  Serial.println("Calibration Complete!");
  //*********************************************************************************

  // Turn off torque when configuring items in EEPROM area (saved even after power loss)
  // Iterate through each motor ID in the DXL_IDs array
  for (size_t i = 0; i < sizeof(DXL_IDs) / sizeof(DXL_IDs[0]); i++) {
      // Set torque off for the current motor ID
      setTorqueOff(dxl, DXL_IDs[i]);

      // Set operating mode for the current motor ID
      setOperatingMode(dxl, DXL_IDs[i], OP_EXTENDED_POSITION);

      // Set torque on for the current motor ID
      setTorqueOn(dxl, DXL_IDs[i]);
  }
  // Speed call - Required every time Operating Mode is changed
  float speedPct = 50.0; 
  for (int i = 0; i < sizeof(DXL_IDs); i++) {
    if (!setSpeed(dxl, DXL_IDs[i], speedPct)) {
        Serial.println("Failed to set speed");
    }
  }

  // Enables pins for user buttons
  pinMode(switchPin1, INPUT_PULLUP);
  pinMode(switchPin2, INPUT_PULLUP);
  pinMode(switchPin3, INPUT_PULLUP);
  
}

void loop() {

  int position1 = digitalRead(switchPin1);
  int position2 = digitalRead(switchPin2);
  int position3 = digitalRead(switchPin3);

    // Position 1
    if (position1 == HIGH && position2 == LOW && position3 == LOW) {
        // Set basePosition to 2048 (zero position) for both motors
        int basePosition = 2048;

        // Move DXL_IDs[0] in a clockwise direction by adding an offset
        int offsetClockwise = 100;
        int goalPosition1 = basePosition + offsetClockwise;

        // Move DXL_IDs[1] in a counter-clockwise direction by subtracting an offset
        int offsetCounterClockwise = 100;
        int goalPosition2 = basePosition - offsetCounterClockwise;

        // Set goal positions for both motors
        moveToPosition(dxl, DXL_IDs[0], goalPosition1);
        moveToPosition(dxl, DXL_IDs[1], goalPosition2);
    }
    // Position 2
    else if (position1 == LOW && position2 == HIGH && position3 == LOW) {
        // Set basePosition to pos2_center for both motors
        int basePosition = pos2_center;

        // Move DXL_IDs[0] in a clockwise direction by adding an offset
        int offsetClockwise = 100;
        int goalPosition1 = basePosition + offsetClockwise;

        // Move DXL_IDs[1] in a counter-clockwise direction by subtracting an offset
        int offsetCounterClockwise = 100;
        int goalPosition2 = basePosition - offsetCounterClockwise;

        // Set goal positions for both motors
        moveToPosition(dxl, DXL_IDs[0], goalPosition1);
        moveToPosition(dxl, DXL_IDs[1], goalPosition2);
    }
    // Position 3
    else if (position1 == LOW && position2 == LOW && position3 == HIGH) {
        // Set basePosition to pos3_center for both motors
        int basePosition = pos3_center;

        // Move DXL_IDs[0] in a clockwise direction by adding an offset
        int offsetClockwise = 100;
        int goalPosition1 = basePosition + offsetClockwise;

        // Move DXL_IDs[1] in a counter-clockwise direction by subtracting an offset
        int offsetCounterClockwise = 100;
        int goalPosition2 = basePosition - offsetCounterClockwise;

        // Set goal positions for both motors
        moveToPosition(dxl, DXL_IDs[0], goalPosition1);
        moveToPosition(dxl, DXL_IDs[1], goalPosition2);
    }

    delay(100); // Add some delay if needed
}

bool moveToPosition(Dynamixel2Arduino &dxl, const uint8_t DXL_IDs, int goalPosition) {
    int presentPosition = dxl.getPresentPosition(DXL_IDs);

    if (presentPosition != goalPosition) {
        // Add a delay to ensure the motor is ready to receive commands
        delay(100);

        // Set the motor to the goal position
        return dxl.setGoalPosition(DXL_IDs, goalPosition);
    }

    // If the motor is already at the goal position, return true
    return true;
}

// Custom velocity function - ENTER speed percentage in SETUP()
bool setSpeed(Dynamixel2Arduino &dxl, uint8_t DXL_3, float speedPct) 
{
    double maxDynamixelSpeed = 1023 * 0.229; //RPM
    uint32_t newSpeedRpm = speedPct * maxDynamixelSpeed / 100;
    uint32_t writeTimeout = 100; // ms
    return dxl.writeControlTableItem(PROFILE_VELOCITY, DXL_3, newSpeedRpm, writeTimeout);
}

// Custom calibration function - Ensures home position is found on startup
bool moveToZero(Dynamixel2Arduino &dxl, const uint8_t DXL_IDs) {
    
    int present_position = dxl.getPresentPosition(DXL_IDs);

    if (present_position != 2048) {
      int direction = (present_position > 2048) ? -1 : 1;

        // Delay for motor to calibrate once power returns
        delay(2000);

        // Calculate the goal position (zero position)
        int goal_position = 2048 - (direction * (abs(2048 - present_position)));

        // Set the motor to the zero position (2048)
        return dxl.setGoalPosition(DXL_IDs, 2048);
    }

    // If the motor is already at the zero position, return true
    return true;
}
