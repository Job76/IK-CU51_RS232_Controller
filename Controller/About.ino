/* -------[ About ]--------------------------------------------------------------------------
Project             : Toshiba IK-CU51 Camera Controller Unit, 
                      RS-232C Communication Protocol,
                      Control Interface
File                : Controller.ino
Version             : 0.50
Started             : 2013-05-21
Updated             : 2013-05-28
Author              : Johan Broström
E-Mail              : johan.brostrom@inspektionssystem.se
Company             : InspektionsSystem AB
Homepage            : www.inspektionssystem.eu

Comments            : Software for Arduino Uno, with MAX202, on custom shield, to control camera
Chip type           : Arduino Uno
Program type        : Application
Clock frequency     : 16 MHz
------------------------------------------------------------------------------------------ */

/* -------[ Revision History ]---------------------------------------------------------------
Legend: 
    B = Bug 
    M = Missing 
    F = Fixed
---------------------------------------------------------------------------------------------    

2013-05-21 - Version 0.01 - Finished custom shield hardware, and started with the software
                            Used PC to check communication and debug

2013-05-22 - Version 0.02 - Added alot of code, still very basic and unfinished
                            B = Due to fact that I'm using a PC instead of a camera, all signals needs to be changed.
                            M = Error handling and time-out
                            M = Set control point, for sending only once, unless there is an error! 
                            M = Set a 2 second time-out waiting time

2013-05-24 - Version 0.10 - Added more code, still very basic. Restructured the code and added code for releasing the switch
                            B = Due to fact that I'm using a PC instead of a camera, all signals needs to be changed.
                            F = Error handling and time-out
                            F = Set control point, for sending only once, unless there is an error! 
                            F = Set a 2 second time-out waiting time

2013-05-24 - Version 0.20 - This should work good enough to make a first test against a real camera control unit. 
                            Must update and fix the bug below before testing. This can be temporarily fixed on the break-out box.
                            Either way, the bug still remains! 
                            B = Due to fact that I'm using a PC instead of a camera, all signals needs to be changed.

2013-05-27 - Version 0.30 - First sharp version, that have been tested and function against the camera 
                            Added Red and Green LED's for fault indication
                            Edited schematics and edited wire drawings on hardware, to match a "true" PC. 
                            F = Due to fact that I'm using a PC instead of a camera, all signals needs to be changed.
                            B = Very slow respons on switch. Need to check time-out and delays
                            
2013-05-27 - Version 0.40 - Removed all debug code, and compacted the code in size. 
                            Bugs from v0.30 not yet fixed, but removing the debug code is a step on the way due to their included delays
                            B = Very slow respons on switch. Need to check time-out and delays                           

2013-05-28 - Version 0.50 - Restructured the code and set a few new IF and AND statments to speed up things and skip the 2s counter delay. 
                            (no new bugs discovered at this time)
                            F = Very slow respons on switch. 
                            
------------------------------------------------------------------------------------------ */


/* -------[ Notes! ]-------------------------------------------------------------------------
 Correct software and hardware, but the TxD and RxD on the schematic diagram is switched. 
 Need to correct this a.s.a.p. 
 
 Testing to change the Uno with a Nano, and make a new circuit board
 
 Also, at version 0.50, a namne change of the source code was made from SerialPort_Test to Controller
 
------------------------------------------------------------------------------------------ */   
