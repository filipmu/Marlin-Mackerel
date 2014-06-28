==========================
Mackerel - Firmware for Lyman Extruder Controller
==========================
This software has a GPL license.
Please do not use this code in products (3D printers, CNC etc) that are closed source or are crippled by a patent.
https://gnu.org/licenses/gpl.html


Quick Information
===================
This firmware was designed in conjunction with Hugh Lyman to control a Lyman filament extruder with geared stepper motors.  It is currently designed to support the RAMPS 1.4 with Smart LCD 2004 and depends on a prototype filament width sensor. It replaces much of the control electronics in the current design with one controller.  It can control the extruder motor, puller motor, Extruder heater (PID) with thermistor, winder motor, filament cooling fan and has input for a filament width sensor.  The processes and menus have been adapted to control a filament extruder.  This code is currently in design, development, and testing and so might not be ready for casual users.



This firmware is based on Marlin 3D Printer Firmware but also has many original parts.

Marlins eat Mackerels and 3D printers consume filament.


Features:
=========

*   Allows complete control of a Lyman Filament Extruder via LCD menus.
*   Controls an extruder screw motor (geared stepper motor) so that it runs at a fixed RPM that can be set with the LCD.
*   Controls a Puller motors (geared stepper motor) calibrated in mm/sec.  Can also be quickly adjusted with the LCD.
*   Accurate temperature control of extruder barrel (Uses Marlin temperature PID control and allows for auto-tuning)
*   With the filament diameter sensor, accurate filament diameter control.
*   Displays statistics (average, min, max) of filament diameter.
*   Displays length of filament extruded.
*   Allows selection of different thermistors (like Marlin)
*   Can connect with Pronterface or Repetier Host to view the extruder temperature trending curves.
*   Smooth motion control of extruder and puller geared stepper motors (Uses the Marlin motion control)
*   Has an input for mechanical switch to allow quick control of the extruder motor.
*   Parameters saved in EEPROM (like Marlin).
*   Ability to control speed of the brushed DC winder motor.
*   ON/OFF control of the filament cooling fan.

The default baudrate is 250000. This baudrate has less jitter and hence errors than the usual 115200 baud, but is less supported by drivers and host-environments.


Limitations and Possible Improvements
=====================================


G-code and M-code control of the firmware is limited to M303 Autotune.  More work is needed to make the codes relevant to a Lyman filament extruder.

This sofware only supports the RAMPS 1.4 and SMART LCD 2004.  More work is needed to make it compatible with other hardware.  Much of the Marlin code that handled board selection is still there, but needs to be updated.

Software requires a filament diameter sensor: http://www.thingiverse.com/thing:89044 .  Work is needed to make the software flexible to run without this feature.

Motor speed control is handled by generating a sequence of position instructions in the code.  It would be better to control the speed of the motors directly.  This would require changing the guts of the step pulsing routine.  I wanted to keep that intact.

Filament control is with a PID loop, but there is dead time in between where the filament is extruded and where it is measured.  A Smith Predictor would be a better way to control it.  The firmware has untested Smith Predictor code but I commented it out since it seemed more complicated to tune (more parameters in additon to PID) and may not be as robust.

Only uses 2 stepper outputs, so there is room for leveraging the other 3 outputs to control other steppers.

Add means to save run statistics in the SD card.  Could capture system data at regular intervals for analysis later.




Implemented G Codes:
====================

*  None

M Codes
=======

*  M303 - PID relay autotune S<temperature> sets the target temperature. (default target temperature = 150C)



Configuring and compilation:
============================

Install the arduino software IDE/toolset v1.x.x
   http://www.arduino.cc/en/Main/Software

Download the firmware
   https://github.com/filipmu/Marlin-Marlin_v1-Lyman-Extruder/
   Use the "Download Zip" button on the right.



Start the arduino IDE.
Select Tools -> Board -> Arduino Mega 2560    
Select the correct serial port in Tools ->Serial Port
Open Marlin.pde

Click the Verify/Compile button

Click the Upload button
If all goes well the firmware is uploading

Running the Lyman Extruder
==========================

See http://www.thingiverse.com/thing:265375 for the extruder build and operation.


1. Turn on the heater – select Preheat ABS from the Main Menu.
2. Wait until temp hits setpoint (185 deg).  Beeper will go.  Can press knob to shut off.
3. Turn on the motors – select  Resume Extruder from the Main Menu.
4. Adjust the Puller motor speed via the knob to achieve close to the filament diameter desired (2.8mm).  Can use the Tune menu or Control menu to make adjustments to motor speeds, etc.
5. Switch to Extruder Auto Con from the Main Menu to  go to automatic control of the puller motor.
6. Can Clear Statistics on the Main Menu to start collecting data now that filament is under control.
7. Select Pause Extruder on the Main Menu when finished with run.
8. Select Cooldown on the Main Menu to shut off heater.





