# IC35Basic
A basic compiler for the Siemens IC35, the unifier 
![image](https://github.com/user-attachments/assets/723c16b6-44f5-486e-9fb3-a6aaea6d63a0)

The Siemens IC35 (the unifier) was a pocket computer driven by Z80 with 240x160 Pixel black and white LC display, 2 MB memory using bank switching, two SD card slots, a smartcard slot, infrared interface and cable interface.

The BASIC compiler compiles the BASIC source code into a pcode for a virtual CPU (one address stack machine) which then is executed. Most opcodes are using Z80 assembler on the machine but are implemented in C on the simulator.

# Installation
If you want to run everything on the real device, just download the MMCard.zip and copy the files to your memory card.
In order to run the simulator or even modify the compiler, you need the IC35 SDK. You may find it here: https://web.archive.org/web/20001001000000*/www.ic35.de
The SDK will install itself into c:\ic35sdk
Download the memory card (MMCard1.zip) and extract it to c:\MMCard1 or any other drive.
You now can start ICBasic.exe which includes the BASIC compiler as well as the simulator. It must run at the same drive where you have installed the MMCard.
If everything is done, ICBasic will present a file selector box.

You may do everything in a virtual machine, for example under Windows XP under VirtualBox.
In order to compile the compiler, you need Visual C 6.0 or Visual studio 6.0, Microsoft released a cost free version but usually you need a serial number. You may find everything on sites with abandoned ware.
You can find Visual Studio here: https://winworldpc.com/product/microsoft-visual-stu/60 

If you want to use visual Studio 6, you should do this on an old Windows like Windows XP in a virtual machine. It will not run properly on modern Windows.

# Programming
In the menu Compiler in ICBasic you find CompileLoop. Every time you press a button, the source code is loaded, compiled and can be run. This is completly useless on the device itself but if you are working in Windows, you can open your BASIC source code in an editor like Notepad++, save it and press the button in the simulator.



