rem open "COM" opens COM in standard mode
rem open "COM:9600,n,8,1" opens the com in 9600 
rem open "IRDA" opens irda in standard mode
rem open "IRDA:9600,n,8,1" opens irda with para
dim key as integer

open "IRDA:" for output as #1
loop:
  print #1,"Hallo"
  key = getkey
  if key = 18 then 
     close 1
     halt
  end if
goto loop
  close 1
 