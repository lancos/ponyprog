-------------------------------------------
PonyProg - Serial Device Programmer  v1.17h

PonyProg comes with a user friendly GUI framework available for Windows95/98,
Windows2000, WindowsNT and Intel Linux.
Its purpose is reading and writing every serial device.
At the moment it supports I²C Bus, Microwire, and SPI eeprom and the flash
ATMEL AVR AT90SXXXX, AT89SXX microcontrollers, and PIC micro.
PonyProg needs a very cheap and simple hardware. But if you don't want to make it
yourself, you can email me and ask for a pcb ready to use.
You can send well documented bug reports to
lancos@libero.it with the subject "PROG BUG REPORT". 

--------
Features
- Support 2402, 2404, 2408, 2416 I²C Bus EEPROM.
- Auto detect 24XX EEPROM capacity 
- Support 2432, 2464, 2465, 24128, 24256, 24512 I²C Bus EEPROM.
- Support 24325, 24645 I²C Bus EEPROM.
- Support Siemens SDE2526, SDA2546, SDA2586, SDA3546, SDA3586   EEPROM (as 24XX Auto).
- Support Siemens SDE2506
- Detect the bank roll over capability of some old 24XX EEPROM.
- Support AT90S1200, AT90S2313, AT90S4414, AT90S8515 Flash microcontrollers
- Support AT90S2323, AT90S2343, AT90S2333, AT90S4433, AT90S4434, AT90S8535
  and AT90S8534 mcrocontrollers
- Support AVR mega microcontroller
- Support AVR tiny microcontroller (not tested)
- Auto detect AVR microcontroller type
- Write lock bits to protect the AVR microcontroller from reading
- Write both the Flash and EEPROM memory of the AVR micro in one shot
- Support the AT89S8252 and AT89S53 micro
- Support 93C06, 93C46, 93C56, 93C66, 93C76, 93C86 Microwire EEPROM (C and LC series,
  the CS serie is not supported yet).
- Support 93C13 (as a 93C06) and 93C14 (as a 93C46) Microwire EEPROM
- Support PIC 16C84/16F84 and PIC16F84A microcontroller
- Support 25010, 25020, 25040 SPI EEPROM
- Support 25080, 25160, 25320, 25640, 25642, 25128, 25256, 95640 Big SPI EEPROM
- Read Intel hex format file as well as raw binary file.
- Read Motorola S-record format file
- Features a custom E²P format file to store the EEPROM characteristics, an editable
  comment and memory content all together with CRC. 
- Enhanced buffer edit, text and hexadecimal.
- Work with Windows95, Windows98, Windows2000, WindowsNT and Linux.
- Work with "ludipipo" and AVR ISP (STK200/300) hardware.
- Easily adaptable to any new hardware interface.


=======
TODO (PonyProg is still under development)
=======
- Add 8 bit organization microwire eeprom
- Add 24C01 support
- Add NVM3060 (IMBUS)
- Setup dialog to change parameters in INI file
- Write flash and eeprom separately in the AVR
- Add color to highlight differences after a verify
- Add remote control mode for Build/Makefile support
- Add PIC16F8XX and PIC12C50x support
- Add autoprogram command (like AVR ISP)
- Add multilanguage support
- Add SX Parallax micro
... any suggestions are welcome

=======
History
=======

v 1.17h
-------
Fixed the "Verify failed" message at the end of PIC16F84A
programming.

v 1.17g
-------
Fixed a problem in loading HEX files for PIC generated
by MPAsm.
Extension appended instead of replaced (Windows only).

v 1.17f
-------
Fixed another problem with the new driver for Win2K.
Fixed a load problem with Motorola S-record files.

v 1.17e
-------
Changed the driver for direct I/O, the old one work only 
with WinNT, while the new one work on Win2000 too (tested
by Luis Frigon, thanks).
Added the "AVRISP I/O" interface, and renamed the old one to
"AVRISP API". WinNT/2000 users have to select "AVRISP I/O"

v 1.17c
-------
Added ponyprog.sys driver for direct I/O on WinNT and Win2000.
Fixed some bugs in IntelHex and MotorolaSRec file loading.
Now generate a S2 format for ATmega. Lengthened the reset delay
for PIC.

v 1.17a
-------
Added the Erase command. At the moment work only for AVR
AT89S and PIC. Corrected the lock programming of AT89S.
Use the hardware timer for usec delay on fast machines.

v 1.16h
-------
Patch release. Corrected a bug in the AT89S53 routine.
Modified the default BigSPIPageWrite value to 16 to make
the 25080 and 25160 write succesfull.

v 1.16g
-------
Patch release. Corrected some severe bugs, reading PIC16F84,
writing AT89S8252 and verifing 24C32 and upper devices.

v 1.16b
-------
Modified BogoMIPS calibration routine (Windows).
Added full Security and Fuse bit support for AVR
and PIC. Some other minor improvement.

v 1.15c
-------
AVR: Timeout bug fixed, and added the probe
before any read/write, with retry capability.
Some Linux version bug fixes.

v 1.15i
-------
Added some error messages.
Some bug fixes.
Added all AVR devices.
Configurable programming delay for the AVR (only in the .INI
file).

v 1.15a
-------
Some bug fixes in the 25640 write routine and menu selection.
Automatic timing calibration. Unified the calibration for all
devices and buses, so modified the I2CBus routine. Bug fixed 
in I2CBus Stop routine.
Some speed improvement (mainly "SI Prog I/O" interface).
Added the "Retry, Abort, Ignore" dialog when the AVR id is
wrong (Device missing, device locked or pre-production device).

v 1.14b
-------
Added the "ClearBuffer" and "FillBuffer" utility.
Added AVR reset retries.

v 1.13h
-------
Setup file for Windows.
Improvements of Big SPI routines. Now it should work fine with
the 25640, 25642 and 95640.
Some corrections and improvements provided by Uwe Bonnes.

v 1.13e
-------
Corrected the AT89Sxx timeout bug, and a bug in the Big SPI
eeprom routines.

v 1.13d
-------
Corrected a bug in the read/write I2CBus eeproms like 24C645 and
24C256 introduced in v1.13b.

v 1.13c
-------
Improved the ATmega programming routines.
Corrected a bug in the selection of the EasyI2C interface in
the setup dialog.

v 1.13b
-------
Added the Big SPI eeprom, the AT89Sxx micro and the 24XX5 eeprom.
Added the motorola S-Record format (only read, not write).
Corrected a bug in the AVRmega programming due to the poor documentation.

v 1.12r
-------
Added the capability to inverts at run-time all the line (thanks to Uwe Bonnes).
Now work with "ludipipo" hardware. Added the AVR ISP parallel interface.
The test in the configuration dialog now works. (But need a little 
modification at the hardware: PIN 6 and 7 connected together, look at
the schematics for more info).

v 1.12h
-------
Added the SDE2506 eeprom.

v 1.12g
-------
PonyProg can use the API or direct I/O (selectable through the
interface dialog). direct I/O is faster than API, but don't
work with Windows NT.

v 1.12e
-------
Added the abort button and the progress bar during the read/write operations.
Corrected a bug that slowing the read of 2432-512 eeproms.

v 1.12c
-------
Now if a serial port is not available the COM radio button is not
activated, and if you try to read/write report an error.

v 1.12b
-------
Added some shortcuts.
Corrected a bug when loading of a file from the command line
argument (or drag a file on the PonyProg icon). Now it ask to
save a modified buffer also when you load a new file.

v 1.12a
-------
Added the lock and reload icons in the toolbar.
Added the two last opened file in the file menu.
Now record the device type in the .INI file.

v 1.11b
-------
Now it ask if you want to save when exit the program with a modified
buffer. Improved the AVR programming speed.

v 1.11a
-------
COM access through the API's. No more use direct port I/O.
Open the serial port before any op and close it at the end, so
the mouse don't freeze anymore with some PC. Added the multibyte
edit facilities. Changed the PIC read/write algorithms.
Added text colors.

v 1.10f
-------
Added the text edit buffer (by clicking in the ASCII rapresentation)
Added the * in the status bar for a modified buffer.

v 1.10d
-------
Added some SPI eeproms.
Corrected a bug in the edit buffer introduced in v1.10c.

v 1.10c
-------
Now the 24C65 Write security and Write high endurance block are
fully usable. Added the dialog box to enter these value.

v 1.10b
-------
Added the PIC16F84/PIC16C84. Corrected the Microwire reading
algorithm to extend the compatibility to all the 93Cx6 eeproms.

v 1.09j
-------
Added tooltip-text. Added programming lock bits in the AVR. Autodetect
AVR type. Some bug fixes.

v 1.09b
-------
Added support for microwire eeproms, and support for security bits in
the 24C65 model.

v 1.08b
-------
Corrected a "swap byte into word bug" for Atmel AVR. The intel hex
format is little endian, so PonyProg have to be too.


-----------------------------------
Copyright, Licensing and Donations

PonyProg is copyright by Claudio Lanconelli. PonyProg is free
software, you can freely copy and redistribute it if no fee is
charged for use, copying or distribution. Is not required any
payment to the author; however the author would welcome any
donations.

PONYPROG IS PROVIDED TO YOU "AS IS," WITHOUT
WARRANTY. THERE IS NO WARRANTY FOR THE
PROGRAM, EITHER EXPRESSED OR IMPLIED,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
THIRD PARTY RIGHTS. THE ENTIRE RISK AS TO THE
QUALITY AND PERFORMANCE OF THE PROGRAM IS
WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE,
YOU ASSUME THE COST OF ALL NECESSARY
SERVICING, REPAIR OR CORRECTION.

IN NO EVENT THE AUTHOR WILL BE LIABLE TO YOU FOR
DAMAGES, INCLUDING ANY GENERAL, SPECIAL,
INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING
OUT OF THE USE OR INABILITY TO USE THE PROGRAM
(INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR
DATA BEING RENDERED INACCURATE OR LOSSES
SUSTAINED BY YOU OR THIRD PARTIES OR A FAILURE
OF THE PROGRAM TO OPERATE WITH ANY OTHER
PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY
HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
DAMAGES.

PonyProg is statically linked with the V library. The V library is
copyright by Bruce E.Wampler and it's distributed under a
different license, the GNU GPL. You can find the V library and
related license at http://www.objectcentral.com.

-----------------------------
Downloading and Installing

PonyProg for Windows95
Run setup.exe and follow the instructions.

If you are updating from an older release deinstall it before
to install the new release. Be sure to delete the ponyprog.ini
file.

-------------------
Send donations to:

Claudio Lanconelli
via Minardi, 10
48022 Lugo (RA)
Italy

E-mail: lancos@libero.it

Download last version of PonyProg from my home page:
http://www.LancOS.com

if you have problems to reach this site try

http://members.tripod.com/lancos
