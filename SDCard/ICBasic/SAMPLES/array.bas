rem Arraytest 
rem currently, only one dimensional arrays are supported, also they begin at 0
rem (this is for speed reason).
rem for ICBasic. See http://www.outlookfolders.de/ic35
dim barray(31) as byte
dim intarray(31) as integer
dim strarray(31) as string
dim i as integer
dim col as integer
dim lineNr as integer
dim max as integer

cls
print "Testing arrays and while"

max = 30
print "Arrays f�llen"
i = 0 
while i < max
   gotoxy 10,50
   print "i=";i;"  ";max;
   intarray(i) = i*2
   strarray(i) = str(i)
   i = i + 1
wend

print "Ausgabe:"
key = getkey
cls

col = 0
lineNr = 0
for i = 1 to max
   gotoxy col,lineNr
   print "I(";i;")=";intarray(i);
   lineNr = lineNr + 14
   if ( lineNr � 140) then 
		col = col + 48
		if col � 230 then
		    gotoxy 0,150
			print "Press any key";
			key = getkey
			cls
			col = 0
		end if
		lineNr = 0
	end if
next i

gotoxy 0,150
print "Press any key";
key = getkey
cls

col = 0
lineNr = 0
for i = 1 to max
   gotoxy col*48,lineNr*14
   print "S(";i;")=";strarray(i);
   lineNr = lineNr + 1
   if lineNr = 10 then 
		col = col + 1
		if col = 5 then
		    gotoxy 0,150
			print "Press any key";
			key = getkey
			cls
			col = 0
		end if
		lineNr = 0
	end if
next i


end 
  