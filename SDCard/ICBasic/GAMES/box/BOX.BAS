rem
rem Boxworld editor and game
rem (C) 2002 Thomas Quester
rem Original game bei Schlüter
rem
rem This is not a game, it is a code sample
rem

declare sub loadlvl(fn as string)
declare sub loadimgs(p as string)
declare sub play()
declare sub showlvl()
declare sub MoveMan(dx as integer, dy as integer)
declare sub LoadLevel(l as integer)
declare sub Menu()
declare sub ChangeLevel()
declare sub Info()
declare sub EditLevel()
declare sub SaveLevel()
declare function EditMenu() as integer

dim lvl(30,20) as byte
dim level as integer
dim box1 as long
dim box2 as long
dim tuer2 as long
dim man as long
dim key as integer
dim tuerx as integer
dim tuery as integer
dim manx as integer
dim many as integer
dim boxes as integer
dim path as string
dim fname as string

path = "MMCard1\boxworld\"

loadimgs "MMCard1\ICBasic\games\box\"

fname = PickFile("MMCard1\boxworld",fname,"BOX","Lade einen Level")

Loadlvl fname 

showlvl 
play

halt

sub loadimgs(p as string)
  print "Loading images ";
  box1=loadimage(p+"box1.img",0)
  print ".";
  box2=loadimage(p+"box2.img",0)
  print ".";
  tuer2=loadimage(p+"tuer2.img",0)
  print ".";
  man = loadimage(p+"Man.img",0)
end sub

sub loadlvl(fn as string)
  dim a as string
  dim i as byte
  dim l as byte
  dim c as byte
  dim mx as byte
  dim my as byte
  dim lvlname as string


  lvlname = right(fn,7)
  lvlname = left(lvlname,3)
  level = val(lvlname)
  open fn for input as #1
  l = 0
  manx = 0-1
  many = 0-1
  while eof(1) = 0
    input #1,a
    for i=1 to len(a)
          c = asc(mid(a,i,1))-48
		  if c = 3 then 
		     manx = i-1
			 many = l
			 c = 0
		  end if
		  if c = 4 then 
		     mx = i
			 mx = l
		  end if

		  lvl(i-1,l) = c
     next i
     l = l + 1
  wend
  close #1
  if manx = 0-1 then
     manx = x
	 many = y
  end if
     
end sub

sub LoadLevel(l as integer)
  dim fname as string

  level = l
  fname = str(l)
  while len(fname) < 3 
      fname = "0" + fname
  wend
  fname = path + "Level" + fname+".box"
  loadlvl fname
end sub

sub showlvl()
  dim x as byte
  dim y as byte
  dim c as byte
  dim i as byte
  dim img as byte
  cls
  boxes = 0
  rem get the level number

  for x = 0 to 29
     for y = 0 to 20
           c = lvl(x,y)
           img = 0
           if c = 1 then img =  box1
           if c = 2 then
		      img = box2
			  boxes = boxes + 1
		   end if
           if c = 4 then 
               tuerx = x
               tuery = y
               img = tuer2
            end if
            if img <>0 then putimage img,x*8,y*8
    next y
  next x
end sub   
  
sub play()
  dim key as integer
  dim finish as integer
  dim s as string
  finish = 0
  BackupLCD 0,0,239,149
  s = "Level " + str(level)
  button s,80,80,0
  key = getkey
  RestoreLCD 0,0,239,149
  while 1 
	  while finish = 0
		 fillblock 0,manx*8,many*8,8,8
		 putimage man,manx*8,many*8 
		 key = getkey
		 gotoxy 0,0
		 if key = 18 then halt
		 if key = 32 then ShowLvl
		 if key = 1 then MoveMan 0-1,0
		 if key = 2 then MoveMan 1,0
		 if key = 3 then MoveMan 0,0-1
		 if key = 4 then MoveMan 0,1 
		 if key = 12 then Menu
		 if boxes = 0 then finish = 1
	  wend
	  gotoxy 80,80 
	  print "Level finished"
	  key = getkey
	  if key = 18 then halt
	  level = level + 1
	  LoadLevel level
	  ShowLvl
	  finish = 0
  wend
  
  key = getkey
end sub

sub MoveMan(dx as integer, dy as integer)
    dim x as integer
	dim y as integer
	dim err as integer
    fillblock 0,manx*8,many*8,8,8
    x = manx + dx
	y = many + dy
	err = 0
	if x < 0 then err = 1
	if y < 0 then err = 1
	if x > 29 then err = 1
	if y > 19 then err = 1

	if err = 0 then
	   if lvl(x,y) = 0 then
		   manx = x
		   many = y

	   else
		   if lvl(x,y) = 2 then
			  if lvl(x+dx,y+dy) = 0 then
			     lvl(x+dx,y+dy) = 2
				 lvl(x,y) = 0
				 putimage box2,(x+dx)*8,(y+dy)*8
				 manx = x
				 many = y
			   end if
			end if
			if lvl(x+dx,y+dy) = 4 then
			   lvl(x,y) = 0
			   gotoxy 0,20
			   fillblock 0,x*8,y*8,8,8
			   manx = x
			   many = y
			   boxes = boxes - 1
			end if
	   end if
	end if
end sub

sub ChangeLevel()
  InputBox "Level",level,0,10,10,30,70
  LoadLevel level
  ShowLvl 
end sub

sub Menu()
  dim s as string
  dim choice  as integer
  s = "010Datei^020Level^030Info^#100Ende^200Level^210Edit^300Info^"

  choice = StringMenu(s,0)
  if choice = 900 then ChangeLevel
  if choice = 930 then EditLevel
  if choice = 0   then halt
  if choice = 1800 then Info
  gotoxy 0,20
end sub

sub Info()
   dim key as integer
   fillblock 0,50,30,120,62
   ShadowEdgeRect 50,30,120,62
   gotoxy 52,32: print "BasicBox V 1.0"
   gotoxy 52,46: print "(C) 2002 Thomas Quester"
   gotoxy 52,62: print "Written in QICBasic"
   key = getkey
   ShowLvl
end sub

sub EditLevel() 
  dim wantExit as byte
  dim key as integer
  dim x as integer
  dim y as integer
  dim img as long
  x = 1
  y = 1
  
  wantExit = 0
  while wantExit = 0
     gotoxy x*8,y*8
	 CursorShape 8,8
	 CursorStatus 1
	 fillblock 0,manx*8,many*8,8,8
	 putimage man,manx*8,many*8 
     key = getkey
	 CursorStatus 0
	 img = 0
	 if key = 12 then 

       key = EditMenu()
    end if 
	 if key = ' ' then
	    if lvl(x,y) = 0 then 
		   lvl(x,y) = 1
		   img = box1
		else
		   lvl(x,y) = 0
		   fillblock 0,x*8,y*8,8,8	
	    end if
	 end if
	 if key= 'b' then 
	    if lvl(x,y) = 2 then 
		   lvl(x,y) = 0
		   fillblock 0,x*8,y*8,8,8
		else
		   lvl(x,y) = 2
		   img = box2
		end if
	 end if
	 if key = 't' then 
	    lvl(x,y) = 4
		img = tuer2
	 end if
	 if key = 'm' then
	    fillblock 0,manx*8,many*8,8,8
	    manx = x
		many = y
	 end if
	 if key = 18 then WantExit=1
	 if key = 1 then 
	    x = x - 1 
		if x < 0 then y = 0
	 end if
	 if key = 2 then 
	    x = x + 1 
		if x > 30 then y = 30
	 end if
	 if key = 3 then 
	    y = y - 1 
		if y < 0 then y = 0
	 end if
	 if key = 4 then 
	    y = y + 1 
		if y > 20 then y = 20
	 end if
    
 	 if img <> 0 then putimage img,x*8,y*8
  wend
end sub


function EditMenu() as integer
   dim s as string
   dim choice as integer
   dim key as integer
   dim x as byte
   dim y as byte

   s = "010Datei^020Edit^#100Neu^110Speichern^200Wand (space)^210Box (b)^220Man (m)^"
   choice = StringMenu(s,0)
   key = 0

   if choice = 0   then
      for x = 0 to 29
	     for y = 0 to 19
		     lvl(x,y) = 0
	     next y 
	   next x
	   for x = 0 to 29
	       lvl(x,0) = 1
		   lvl(x,19) = 1
	   next x
	   for y = 0 to 19
	       lvl(0,y) = 1
		   lvl(29,y) = 1
	   next y
	   ShowLvl
   end if
   if choice = 30  then SaveLevel
   if choice = 900 then key = ' ';
   if choice = 930 then key = 'b';
   if choice = 960 then key = 'm';
   EditMenu = key
end function

sub SaveLevel()
    dim newlvl as integer
	dim fname as string
	dim s as string
	dim x as integer
	dim y as integer
	newlvl = level
    InputBox "Level",newlvl,0,10,10,30,70
    fname = str(newlvl)
    while len(fname) < 3 
        fname = "0" + fname
    wend
    fname = path + "Level" + fname+".box"
	cls
	print fname
	open fname for output as #1
	lvl(manx,many)=3
	for y = 0 to 19
	   s = ""
	   for x = 0 to 29
	       s = s + chr(lvl(x,y)+48)
	   next x
	   print s
	   print #1,s
	next y
	close #1
	lvl(manx,many) = 0
	key = getkey
	showlvl
end sub

	   
	

     
      