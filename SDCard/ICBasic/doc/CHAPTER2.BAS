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
     =,�,�,��,�=,�=
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






         
 