# Changelog

## Electronic System Version History:

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