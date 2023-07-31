# Changelog

## Electronic System Version History:

### v0.6.1
- Improvements to success/failure statements, in the event of failure to complete an action, the action will be performed another 3 times before a time out will occur and the program will exit safely.

### v0.6
In v0.6, testing begins using two servos and getting closer to achieving the MVP.
- Pre-processor streamlined to remove clutter, variables adjusted to smaller sizes where possible.
- Added an array DXL_IDs for Dynamixel motors, ID's were updated using update.ino from nikwl's dynamixel-shield-toolbox found here: https://github.com/nikwl/dynamixel-shield-toolbox
- Added ping condition in the event of failure to connect to motor.
- Added iteration for switching operating modes so both motors will be updated.
- Added success/failure statements to Serial monitor.
- Extracted serial commands into function called SerialMon(). This is a non-blocking function so will run concurrently with Dynamixel motors.
- Converted if/else statements for base positions into a bitwise switch statement to improve readibility.
- Utilising PuTTY to connect to Serial Monitor and utilise escape sequences for better control and readibility.


### v0.5
In v0.5, the Dynamixel motors had arrived, spent the next few days tinkering and testing the servos and reading lots of documents understanding how an individual Dynamixel motor worked. The code here is a result of that research and testing. 
- Change to pin positions for rotary switch.
- Base Positions added as variables rather than constants.
- Namespace introduced for Dynamixel control tables
- Static speed value converted to speed function that can be saved to EEPROM.
- Added error conditions in the event speed function failed.
- Switching Dynamixel operating mode for different actions.
- Added calibration phase to setup.
- Improvements in readibility with setGoalPosition used only once in loop().
- Removed zero positioning function, Goal Position (116) does this automatically on power on see Note 3: https://emanual.robotis.com/docs/en/dxl/x/xm430-w210/#goal-position
Reverse feature
- Added negative base positions to imply the change in direction for the servo motor.


### v0.4
In this version we introduce the Dynamixel motors, for our project we have chosen two XM430-W210-R servo actuators with a Dynamixel Shield interfacing into the Arduino Mega 2560. We begin to explore how the motors would work.
However, the motors have not arrived yet. In Dynamixel-flex, code was created for a possible flex sensor that could be used to turn arm movement into control of the servos.

- No longer using movingAvg header file and switched to using Dynamixel libraries.
- It has been decided that due to time constraints to focus all attention on getting the system working with only a dial as a minimum viable product (MVP). So until that has been achieved, the potentiometer development has been put on hold.


### v0.3.1
- A small change where SET positions on the potentiometer control the servo motor tensioning. This will allow for a better understanding of the potentiometer control and we can try this version with theother version and see which one is best.

### v0.3
It was decided in a Team meeting to switch from steppers to servo motors due to power requirements, portability 
and position control. By using a potentiometer we can make a lot of the buttons/switches obsolete as well as
removing the need to home the stepper. 
- Exploring using a potentiometer to provide sensor input from arm movement.
- Due to this change previous version is obsolete.

### v0.25.1
This version has been edited to only work for 2 stepper motors instead of 4
Using 2 Stepper Motors: [Wokwi Simulation v0.25.1](https://wokwi.com/projects/365491902229667841)

### v0.25
There have been several improvements from v0.2, these include:
- Calibration sequence for stepper aligment takes place before the system can be used. 
- Refactoring of code blocks into functions: improving readibility, abstraction & modularity.
- Release the tension by double pressing the button for each arm.
- Non-blocking code allows for simultaneous actions and monitoring in loops.
- Improved performance.

### v0.2
Improvements over v0.1. Listed below:
- Now using new stepper library AccelStepper.h, allows for greater control and configuration of stepper drivers.
- Using C++ objects to define Stepper and Driver components.

See the [open issues](https://github.com/LeonidasEng/Exoskeleton/issues) for a full list of proposed features (and known issues).