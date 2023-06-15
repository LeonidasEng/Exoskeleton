/***********************************************************************************************************************************\
|*                                                           Exogames - Project                                                    *|
|*                                                            Code created by:                                                     *|
|*                                                              LeonidasEng                                                        *|
|*                                                                                                                                 *|
|*                                              Exoskeleton - Electronic System Prototype                                          *|
|*                                                                v0.25.1                                                          *|
\***********************************************************************************************************************************/

/* 
Copyright (c) 2023, LeonidasEng
All rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the root directory of this source tree. 

***********************************************************************************************************************************

Explanation of System Design:
The following code controls 4# Stepper Motors using a4988 constant current drivers. 
Using a system of buttons and sensors, the motors can tension bungee cords to a given speed and position.


Changelog:
v0.25.1
This version has been edited to only work for two stepper motors instead of 4
--- Simulation v0.25.1: https://wokwi.com/projects/365491902229667841

v0.25
There have been several improvements from v0.2, these include:
- Calibration sequence for stepper aligment takes place before the system can be used. 
- Refactoring of code blocks into functions: improving readibility, abstraction & modularity.
- Release the tension by double pressing the button for each arm.
- Non-blocking code allows for simultaneous actions and monitoring in loops.
- Improved performance.

v0.2
Improvements over v0.1. Listed below:
- Now using new stepper library AccelStepper.h, allows for greater control and configuration of stepper drivers.
- Using C++ objects to define Stepper and Driver components.

***********************************************************************************************************************************

*/
#include <Arduino.h> // Required for VSCode IDE
#include <AccelStepper.h> // Changing to AccelStepper to utilise stepper drivers
#include <LiquidCrystal.h> // For Tension Monitoring


/* Definitions used for pre-processor to reduce memory cost */

/* Stepper Constants */
#define macceleration 100 // Maximum Acceleration
#define mspeed 20  // Maximum Speed
#define maxSteps 1000 // Equals 5 revolutions (200 steps per rev)

// Right hand will be dominant hand, so Motor 1 will be lowest PWM pin
// Defining pins for the stepper drivers, this is for A4988 these will need to be changed when new components are sourced
#define motor1_stepPin 2
#define motor1_dirPin 3
#define motor2_stepPin 6
#define motor2_dirPin 7


/* Button Control */
#define pinButton1 22
#define pinButton2 23
byte buttonLastState1;
byte buttonLastState2;

enum { None, ButtonS, ButtonD, YesHolding, ButtonS2, ButtonD2}; 
/* Enumeration defines named integer constants 
None - no button press
ButtonS - one button press
ButtonD - two button press */

// Using Buzzer for notification to the user
#define buzzerPin 24 

/* Component Setup */
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5); // LCD pins are: (RS, E, D0, D1, D2, D3)

/* Other Stepper Drivers that can be used */
//AccelStepper motorX(AccelStepper::FULL4WIRE, AIn1, AIn2, BIn1, BIn2);  // works for TB6612 (Bipolar, constant voltage, H-Bridge motor driver)
//AccelStepper motorX(AccelStepper::FULL4WIRE, In1, In3, In2, In4);    // works for ULN2003 (Unipolar motor driver)

/* A4988 Driver Objects */
AccelStepper motor1(AccelStepper::DRIVER, motor1_stepPin, motor1_dirPin);
AccelStepper motor2(AccelStepper::DRIVER, motor2_stepPin, motor2_dirPin);



/* Position for Serial */
long currentPosition2 = 0;
long currentPosition1 = currentPosition2; 
int checkButton(void);

void setup() 
{
  // initialize the serial port:
  Serial.begin(9600);

  // Set buttons as pull up
  pinMode(pinButton1, INPUT_PULLUP);
  pinMode(pinButton2, INPUT_PULLUP);
  buttonLastState1 = digitalRead(pinButton1);
  buttonLastState2 = digitalRead(pinButton2);

  // Initialise Stepper Motors
  motor1.setMaxSpeed(mspeed);
  motor1.setAcceleration(macceleration);
  motor2.setMaxSpeed(mspeed);
  motor2.setAcceleration(macceleration);

  
  // Calibration of motors to the same origin (ZERO)
  Serial.print("Calibrating");
  for (int i = 0; i < 3; i++)
  { 
    delay(100);
    Serial.print(".");
  }
  
  motor2.move(2);
  while (motor2.distanceToGo() != 0) {
    motor2.run();
  }

  motor1.setCurrentPosition(0); // Set the initial position to 0
  motor2.setCurrentPosition(0);

  delay(1000);
  Serial.println("COMPLETE");
  tone(buzzerPin, 1047, 250);
  delay(300);
  tone(buzzerPin, 1319, 250);
  delay(300);
  tone(buzzerPin, 1568, 150);
  delay(200);
  noTone(buzzerPin);
  // Initialise LCD
  lcd.begin(16, 2);

}
/* ---Void LOOP now has been streamlined into features with set functions for easier integration--- */
void loop() 
{
  SerialMon(); // Serial Monitor call - This feature can be expanded to include user interface

  /* Switch statement is primary control for system */
  switch (checkButton ())  {
    case ButtonS:
      Serial.println ("Holding or Release?");
      break;

    case ButtonD:
      Serial.println ("Resetting to Origin...");
      reset2Origin();
      break;
          
    case YesHolding:
      buttonControl();
      break;

    case ButtonS2:
      Serial.println ("Holding or Release?");
      break;

    case ButtonD2:
      Serial.println ("Resetting to Origin...");
      reset2Origin();
      break;
          
    default:
      //Default does nothing
      break;

  } 
}

int checkButton (void)
{
  /* ---"checkButton" is a feature for monitoring current button state---
    Using long variables for measuring time we can see if the user is holding or double pressing the button */
  const unsigned long buttonTimeout  = 250;
  static unsigned long previousTime;
  unsigned long currentTime = millis();
  
  const int debounceDuration = 100; // Debounce prevents extra activations from input noise 
  static unsigned long  debounceStartTime = 0;

  if (previousTime && (currentTime - previousTime) > buttonTimeout)  {
    previousTime = 0;

    return YesHolding; // Returning holding either button down
  }

  byte button1 = digitalRead(pinButton1);
  if (buttonLastState1 != button1)  {
    
    if (millis() - debounceStartTime < debounceDuration) {
      return None; // Debounce is considered here and will return none.
    }
    debounceStartTime = millis(); // assigned current Time
	
    buttonLastState1 = button1;

    if (button1 == LOW)  {   // Press
      if (previousTime)  { // 2nd Press
        previousTime = 0;
        return ButtonD; // Recognises double press as input is in quick succession
      }
      else {
        previousTime = 0 == currentTime ? 1 : currentTime; // if currentTime is 0, previousTime will be set to 1 otherwise set value to currentTime
		
        return ButtonS; //SINGLE?
      }
    }
  }
  // This function is then repeated for Button 2
  byte button2 = digitalRead(pinButton2);
  if (buttonLastState2 != button2)  {
    
    if (millis() - debounceStartTime < debounceDuration) {
      return None;
    }
    debounceStartTime = millis();
	
    buttonLastState2 = button2;

    if (button2 == LOW)  {   // Press
      if (previousTime)  { // 2nd Press
        previousTime = 0;
        return ButtonD2;
      }
      else {
        previousTime = 0 == currentTime ? 1 : currentTime;
		
        return ButtonS2; //SINGLE?
      }
    }
  }

  return None;
}

void SerialMon()
{
  /* This feature will be the main part of further developments, as this could be configured into a user interface */
  static unsigned long lastMillis = 0; // Last Time
  const unsigned long interval = 100;
  if(millis() - lastMillis >= interval)
  {
    // Non-blocking code allows for multiple tasks to be executed simultaneously within void loop
    Serial.print("Motor Position 1: ");
    Serial.print(motor1.currentPosition());
    Serial.print(" | Motor Position 2: ");
    Serial.println(motor2.currentPosition());

    lastMillis = millis();
  }
}

void buttonControl()
{
  /* buttonControl was refactored from void LOOP and placed here to improve modularity and readability */
  Serial.println("Yes Holding - Tension increasing");
  while (digitalRead(pinButton1) == LOW || digitalRead(pinButton2) == LOW)
  {
    // GREEN
    if (digitalRead(pinButton1) == LOW) 
    {
      if (currentPosition1 > -maxSteps)
      {
        motor1.setSpeed(-mspeed);
        motor1.run();
        currentPosition1 = motor1.currentPosition();
      }
    }
    // BLUE
    if (digitalRead(pinButton2) == LOW) 
    {
      if (currentPosition2 < maxSteps)
      {
        motor2.setSpeed(mspeed);
        motor2.run();
        currentPosition2 = motor2.currentPosition();
      }
    }
    else {
      motor1.stop();
      motor2.stop();
    }
  }
}

void reset2Origin() 
{
  /* Reset2Origin is the most complex function so far and requires a detailed explanation:
      - This function is non-blocking code able to run simultaneously for both stepper pairs.
      - We have the ability to find any current position of the motor and return it to the stepper origin(0) with one button gesture.
  */
  AccelStepper* motors[] = {&motor1, &motor2}; // Array contains pointers to motor objects
  bool motorActive[] = {false, false}; // using this bool array we can define active motors and prevent faults (wrong direction or alignment)

  for (int i = 0; i < 2; ++i) // In this for loop we iterate over each motor object finding each current position
  {
    long currentPos = motors[i]->currentPosition();
    //motors[i]->setSpeed(200);  // set the speed of the motor at the start of each function call


    // If the current position is NOT equal to 0 we decide which direction to move the motor 
    // for each pair. Motor1&2 (Right Arm) tension in negative direction (CCW) so will release in positive direction
    // Motor3&4 (Left Arm) tension in positive direction (CW) so will release in negative direction
    if (currentPos != 0) 
    {
      int direction;
      if (i < 2) {
        direction = (currentPos < 0) ? 1 : -1;
      } else {
        direction = (currentPos > 0) ? -1 : 1;
      }
      
      // For each motor within the array we can then set the speed and because we have already calibrated 
      // the stepper motors we can have them return to Origin:
      motors[i]->setSpeed(200 * direction);
      motors[i]->moveTo(0);
      motorActive[i] = true;  // set motor(s) as active
    }
  }
  // With the direction calculated we can set a specific speed for the return (to be tested)
  bool allMotorsAtOrigin = false;
  
  while (!allMotorsAtOrigin) // This will then continue to loop until each motor has reached the origin destination 
  {
    allMotorsAtOrigin = true;
    for (int i = 0; i < 2; ++i) {
      if (motorActive[i] && motors[i]->distanceToGo() != 0) // Each motor must be active and must have a non-zero distance to travel
      {
        if (i < 2 && (motorActive[2] || motorActive[3])) { // This prevents faults from occuring in motor pairs from moving in wrong direction 
          continue;  // skip this iteration
        }
        motors[i]->runSpeed(); // Motor ready to move? Call runSpeed() to step.
        allMotorsAtOrigin = false; // Ensures motors at origin will not move
      } else {
        motorActive[i] = false;  // set motor as inactive if at origin
        motors[i]->setSpeed(200); // can redefine speed value for rest of program
      }
    }
  }
}

/*
References:
[1] https://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html
[2] https://hackaday.io/project/183279-accelstepper-the-missing-manual/details
[3] https://curiousscientist.tech/blog/arduino-accelstepper-tb6600-walkthrough


*/
