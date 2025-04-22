# IC35Basic
A basic compiler for the Siemens IC35, the unifier 
![image](https://github.com/user-attachments/assets/723c16b6-44f5-486e-9fb3-a6aaea6d63a0)
![ICBackToBusiness](https://github.com/user-attachments/assets/241a8bbc-69d9-480e-8ca0-ac6d0da47a98)

The newest version of the source code is still missing, however I could recover the SD card with the binaries, demos and Dokus. I still have a few hundredet CDs and a box of old hard disks left as well as a box full of tapes (but no tape drive) so there is some chance, this will be complete in the future.

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

# QICBasic manual

1 Types and variables

1.1 Basic types

QICBasic supprts 5 basic types: byte, integer, long, 
float and string.

If variables are not declared, they are created 
automatically on their first use. The type can be
given by a postfix: 

    no prefix    creates a float
    %                   creates an integer
    $                    creates a string

1.2 Explicit variable declaration

The DIM statement is used to create a new variable. It
can be used anywhere in your application. If used inside
a proceure, it will create a local variable.

Each DIM creates only one variable.

Synax:
        DIM name AS typ
        DIM name(dim1,dim2,dim3) AS typ

  Typ may be one of the following:

       BYTE               One byte (0..255)
       INTEGER         Two bytes (-32767..32767)
       LONG                Four bytes (-231..231)
       FLOAT              Four bytes single precision float
       STRING           A string
       usertype       defined by the programmer

Strings may be dynamic or fixed length. Fixed length 
strings are declared using *,for example:

       DIM name AS STRING * 40

without the * 40, name may have any  length.

1.3 Where variables are stored

Global variables are stored in the variable space, which
is created before your program starts. Its size is 
determined on compile time and usually starts one
byte after the code. 
Local variables are stored on the stack.
Fiixed length strings are stored on the heap, which is
located after the varable space.
Absolute variables are layed out on a fixed position
in RAM, the address is determined at compile time. 

1.4 ABSOLUTE variables

For speed reasons, variables can be layed out on a fixed
position in RAM. On the IC35, we use the processor stack
bottom (QICBasic is using only a few bytes on the stack)

ABSOLUTE variables may be layed out on any position
in RAM. Example

    DIM screenline(160) ABSOLUTE &H8000 AS BYTE
    DIM x ABSOLUTE AS BYTE
    DIM y ABSOLUTE AS BYTE
    DIM i AS INTEGER

Screenline is layed out at the address where the IC35
mapps its screen into memory. X and Y are layed out
out at the stack bottom, i is layed out at the variable
space.

Absolute variables are much faster than normal ones, 
because there is no additional address calulation needed.

1.5 TYPE declarations

The TYPE statement is used to declare new types.

Example

 TYPE starship
     coord as vector
     speed as vector
     rockets as byte
     lasers as byte
     fuel as integer
     sprite as long
  END TYPE

  TYPE vector 
     x AS BYTE
     y AS BYTE
  END TYPE

  DIM hero  AS starship
  DIM enemies as starship

To draw your ship, you may enter:

   DRAWSPRITE  hero.sprite,hero.x,hero.y,1

To save your record to a file, it is recommanded to
use fixed strings, because they are stored inside the
type:

   TYPE Taddress
       nr as INTEGER
       name AS STRING * 25
       street AS STING * 25
       city AS STRING * 25
       zip AS INTEGER
   END TYPE

    DIM adr AS Taddress
 ' read record recnr
    SEEK #1,recnr*sizeof(Taddress)+sizeof(TIndex)
    GET #1,adr

1.6 Options

 OPTION  EXPLICIT

Variable declaration is explicit, only. In this mode all
variables are declared via dim. Undefined variables
cause an error on the first pass. 

 OPTION IMPLICIT

Variables are declared on their first use. A dim is not
 nessary. this mode is the default.

 OPTION NAME "name"

Sets the application name for the application loader.
The name may not be longer then 15 characters.

 OPTION ICON "icon.img"

Sets the icon for the application for use in the 
application loader.  The image must be in internal
format wie 16x16 pixels. On the PC you can use the
ICON application to convert a bitmap into an IMG file.

1.8 Speed considerations

In order to speed up your progam, you should use the 
smallest number format that suits your need. For 
example, on the IC35 a Z80 processor is working, which
is a 8 bit machine. Using floats and longs require a
library call on every operation, even on addition of
numbers. Operations on  integer require a lot of code
on loading the numbers before any operations. 
Fastest operations can be performed on the byte type.

In an expression all variables should have the same
type, otherwise the compiler is inserting additional
code to convert the values to a common format
before each operation.

QICBasic manual chapter 2

2.1 Expressions

QICBasic is able to work on for numeric types. During
expression evaluation, all of them can be combined.
The compiler tries to find the fastest way to evaluate
the expressions by calculating sub expressions with
the lowest type. 

Example

 dim b as byte
 dim i as integer
 dim l as long
 dim f as float

b = 255
f = b+1

f contains 0, because the expression is evaluated as
a byte expression, a byte can only hold values up to
255. To get 256, you can cast the value:

f = float(b) + 1

This turns b into a float, then adds one to it. Faster
speed you gain with:

f = integer(b) + 1

since the expression is made in integer, then turned
into  a float.

2.2 Operators


     +, -,*,/   
     mod             Modulo
     and              binary and
     or                 binary or
     =,›,‹,‹›,‹=,›=
                          Compares two values and return 0 or 1
                          as false or true

   On strings, only comare operators and + are valid

2.3 Functions and Subroutines
Subs allow the programmer extending the language 
with their own command and functions.  Each sub or
function must be known before its first use. This is
for speed reasons, the compiler needs at least to
know how many bytes of code to create on its first
pass.
Both functions and subs can use parameters and
local variables. Parameters can be pased as value
or as reference. Value parameters must be basic 
types: byte. integer, long, float and strings. Byref
parameters can be of ownertype as well.

(Note: Byref is currently not supported).

Sample:

 
  declare sub Move(sprites()
  declare function HitTest(x as byte, y as byte) as byte
  declare sub Fire(x as byte, y as byte, dx as byte
                           dy as byte)

   loop:
       MoveSprites
       key = scankey
       if key = 32 then
          Fire(hero.x,hero.y, hero.dx,hero.dy)
       end if
      if HitTest(hero.x,hero.y) then goto GameOver
      goto Loop

sub MoveSprites
     dim i as byte
     for i = 1 to NumSprites
         if sprite(x).visible
            sprite(i).x = sprite().dx
            sprite(i).y = sprite(i).dy
            drawsprite sprite(i).img,sprite(i).x,sprite(i).y,0
         end if
    next i
end sub

function HitTest(x as byte, y as byte) as byte
    dim i as integer
    HitTest = 0
    for ....
          if .... then HitTest = 1
     ..
end function

The dim statement inside a sub or function creates
a local variable,this is allocated on the stack and
removed after the sub or function teminates. It
is also possible to use the paramers for storage of
values. If they are byval parameters, no values
are overridden, if they are byref, the variable 
belonging to them get overridden.

Speed considerations

Subs and functions are a bit slower than gosubs, this
is because the virtual processor needs to perform
additional commands to initialize the parameters and
local variables.  Also the space for local variables is
cleared on each call and disposed afterwards. If 
strings are declared as local variables, for each of
them, some code is created in order to remove it
from the memory on function return.

For use with parameters, subs are preferd, because
on use with gosub. additional code must be written
by the programmer to set global variables before
the gosub.

Conclusion: If you need fAsdst processing on global
variables, use a gosub. If you want a structured
application, which is easier to read, use a sub or
function






         
 

