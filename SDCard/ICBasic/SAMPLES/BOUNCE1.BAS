dim y as integer
dim x as integer
dim rx as integer
dim ry as integer
dim key as integer
dim i as integer
cls
x = 5
y= 5
rx = 1
ry = 1
key = 0
loop:
  key = scankey
  if key = 18 then halt
for i = 1 to 500
  dot 1,x,y
  x = x + rx
  y = y + ry
  if x � 235  then rx=0-rx
  if x �5 then rx = 0- rx
  if y� 150 then ry= 0 - ry
   if y � 5 then ry= 0-ry
next i
goto loop
halt
 