QICBasic manual - Capter 1

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
       LONG                Four bytes (-2�31..2�31)
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






 