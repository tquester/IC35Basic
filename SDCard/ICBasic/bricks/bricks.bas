
dim images(8) as long
dim imgnames(8) as string
dim bricks(26,12) as byte
dim ball as long
dim path as string
dim level as integer
dim scrline as integer
dim field as string
dim key as integer
dim fname as string
dim i as integer
dim x as integer
dim y as integer
dim ix as integer
dim iy as integer
dim tx as integer
dim ty as integer
dim barx as integer
dim imgbar as long
dim imgball as long
dim imgsavball as long
dim imgsavbar as long
dim ballwait as byte
dim ballspeed as byte
dim numBricks as integer
dim maxlevel as integer
dim bx as integer
dim by as integer
dim bsx as integer
dim bsy as integer
dim bsbar as integer
dim brx as integer
dim bry as integer
dim cx as integer
dim cy as integer
dim b as integer
dim bo as byte

dim balllost as byte
dim balls as integer
dim ballsperlevel as integer

rem Set the ball speed


rem Set the relative ball speed n loops for one ball movement
rem set the number of available balls per level

ballspeed = 4
level = 1
maxlevel = 3
ballsperlevel = 5

cls
path = "MMCard1\ICBasic\bricks"
imgnames(0) = "BRICK0"
imgnames(1) = "BRICK1"
imgnames(2) = "BRICK2"
imgnames(3) = "BRICK3"
imgnames(4) = "BRICK4"
imgnames(5) = "BALL1"
imgnames(6) = "BALL2"
imgnames(7) = "BAR"

rem Every brick consists of two parts each is 10x10 pixels, we call them
rem 1,2,3 and 4, 1-2 and 3-4 make up a complete brick
rem each line may have up to 24 bricks

cls
font 3

gotoxy 40,30
print "Bricks 0.3"
gotoxy 40,50
print "Written with ICBasic"
gotoxy 40,70
print "Feb. 2002 by Thomas Quester"
button "Loading images",100,100,0
gosub LoadImages
imgbar = images(7)
imgball = images(5)
imgsavball = allocimage(1024)
imgsavbar  = allocimage(1024)
cls
bx = 100
by = 100
barx = 100

gosub SaveBall
gosub SaveBar
gosub DrawBar
gosub DrawBall
cls

Nochmal:
Gosub MakeLevel
balls = ballsperlevel

rem ---------- Start new level --------------
levelLoop:

Gosub ShowLevel
barx = 20
levelNewBall:
BallLost = 0
by = 135
bx = 50
bry = 0-1
brx = 1
ballLost = 0
gosub SaveBall
gosub SaveBar
bsbar = barx
bsx = bx
bsy = by
gosub DrawBar

levelCont:
brx = 0
bry = 0
bsx = 0-1
while brx = 0
   key = scankey
   gosub HandleKey

   if key = 32 then 
       brx = 1
	   bry = 0-1
	   bx = barx
   end if
wend 

ballwait = ballspeed
while ballLost = 0 
   
   key = scankey
   Gosub HandleKey

   ballwait = ballwait - 1
   if ballwait = 0 then      
      if brx <> 0 then  Gosub DrawBall
      if brx = 0 then brx = 1
	  if bry = 0 then bry = 1
      bx = bx + brx
      by = by + bry
      if (bx < 10) or  (bx > 220) then brx = 0 -brx
      if (by < 10) or  (by > 150) then bry = 0 - bry
      Gosub CheckCollision
	  ballwait = ballspeed
	  
   end if

wend
 if NumBricks < 5 then
    level = level + 1
	if level > maxlevel then level = 1
	ballLost = 0
	balls = balls + 1
	gosub MakeLevel
	goto levelLoop
 end if
 gotoxy 50,50
 print "Ball verloren"
 key = getkey
 balls = balls - 1
 if balls <= 0 then goto lost
 cls
 gosub ShowLevel
 goto levelNewBall

halt

Lost:
    font 3
	gotoxy 80,120
	print "Verloren"
	key = StringDialogbox("Nochmal",0)
	if key = 2 then goto Nochmal
	halt


SaveBar:
	bsbar = barx-15
	getimage imgsavbar,bsbar,136,30,8
	return
DrawBar:
	putimage imgsavbar,bsbar,136
	gosub SaveBar
	putimage imgbar,barx-15,136
	return

DrawBall:
    if bsx > 0 then putimage imgsavball,bsx,bsy
	gosub SaveBall
	putimage imgball,bx-5,by-5
	return

SaveBall:
    
	bsx = bx-5
	bsy = by-5
	bsy = bsy - (bsy and 7)
	getimage imgsavball,bsx,bsy,10,16
	return

HandleKey:
   if key = 18 then halt
   if key = 2 then         
            putimage imgsavbar,bsbar,136
			barx = barx + 5
			if barx > 225 then barx = barx - 5
			gosub DrawBar
   end if
   if key = 1 then  
         putimage imgsavbar,bsbar,136
         barx = barx - 5
  	     if barx < 15 then barx = barx + 5
		 gosub DrawBar
   end if
   return




CheckCollision:
   if (bx > 0) and (by > 0) then
	   cx = bx / 10 
	   cy = by / 10
	   
	   if cy < scrline then
	       if (cx > 1) and (cy > 0) then
			   if bricks(cx,cy) <> 0 then
				  tx = cx * 10 
				  ty = cy * 10 

			      b= bricks(cx,cy)
				  
				  putimage imgsavball,bsx,bsy
 				  putimage images(0),cx*10-5,cy*10-5
				  bsx = 0-1
				  if abs(tx-bx) > 2 then brx = 0-brx
				  if abs(ty-by) > 2 then bry = 0-bry

				  rem find the other part and delete it
				  bo = 0
				  if b = 1 then
				     numBricks = numBricks - 1
					 if bricks(cx+1,cy) <> 0 then numBricks = numBricks - 1
				     bricks(cx,cy) = 0
					 bricks(cx+1,cy) = 0
					 
					 putimage images(0),cx*10-5,cy*10-5
					 putimage images(0),(cx+1)*10-5,cy*10-5
				  end if
				  if b = 2 then 
				     if bricks(cx-1,cy) <> 0 then numBricks = numBricks - 1
					 numBricks = numBricks - 1
				     bricks(cx,cy) = 0
					 bricks(cx-1,cy) = 0				 
					 
					 putimage images(0),cx*10-5,cy*10-5
					 putimage images(0),(cx-1)*10-5,cy*10-5
				  end if
				  if b = 3 then
				     bricks(cx,cy) = 1
					 bricks(cx+1,cy) = 2
					 putimage images(1),cx*10-5,cy*10-5
					 putimage images(2),(cx+1)*10-5,cy*10-5
				  end if
				  if b = 4 then 
				     bricks(cx,cy) = 2
					 bricks(cx-1,cy) = 1
					 putimage images(2),cx*10-5,cy*10-5
					 putimage images(1),(cx-1)*10-5,cy*10-5
				  end if
				  Gosub Sound
			   end if
		       
			   if numBricks < 5 then BallLost = 1
		   end if
	   end if

	   if by > 136 then
		  if (bx < barx-15) or (bx > barx+15) then 
			  gosub Bounce
			  ballLost = 1
		  else 

		      if abs(barx - bx) > 10 then
		         if (bx - barx) > 10 then brx = 2
			     if (barx - bx) > 10 then brx = 0-2
			  else
			     if (bx - barx) < 0 then brx = 0-1 else brx = 1
			  end if
			  bry = 0-bry
			  if brx = 0 then brx = 1
		  end if
 	   end if
	end if
   return

Bounce: 
   bry = 0 - bry
   brx = 0 - brx
   return

MakeLevel:

scrline = 1
   numBricks = 0
   for x = 0 to  23
       for y = 0 to 11
	       bricks(x,y) = 0
	   next y
   next x

      

   if level = 1 then 
		field = "1212121212343434121212"	:gosub StoreLine
		field = "2121212123434341212121"	:gosub StoreLine
		field = "1212121234343412121212"	:gosub StoreLine
		field = "2121234343412121212121"	:gosub StoreLine
		field = "1212343434121212121212"	:gosub StoreLine
   end if
   if level = 2 then 
		field = "0000000121212120000000"	:gosub StoreLine
		field = "0000034121212123400000"	:gosub StoreLine
		field = "0003434121212123434000"	:gosub StoreLine
		field = "0343434121212123434340"	:gosub StoreLine
		field = "0343434121212123434340"	:gosub StoreLine
		field = "0343434121212123434340"	:gosub StoreLine
		field = "0003434121212123434000"	:gosub StoreLine
		field = "0000034121212123400000"	:gosub StoreLine
		field = "0000000121212120000000"	:gosub StoreLine
   if level = 3 then 
		field = "1212121212121212121212"	:gosub StoreLine
		field = "3434343434000034343434"	:gosub StoreLine
		field = "1212121212121212121212"	:gosub StoreLine
		field = "0000000034343400000000"	:gosub StoreLine
		field = "3434343434343434343434"	:gosub StoreLine
		field = "3434343434343434343434"	:gosub StoreLine
   end if
   end if
   return

StoreLine:
   
   for i = 1 to len(field)
       bricks(i,scrline) = asc(mid(field,i,1))-'0'
	   if bricks(i,scrline) <> 0 then numBricks = numBricks + 1
   next i
   scrline = scrline + 1
   return
       
ShowLevel:
   cls
   x = 0
   y = 0
   for iy = 1 to scrline-1
      for ix = 1 to 22
	      rem gotoxy ix*10,iy*12
		  rem print bricks(ix,iy);
	      putimage images(bricks(ix,iy)),ix*10-5,iy*10-5
		  x = x + 10
	  next ix
	  x = 0
	  y = y + 10
   next iy
   font 4
   gotoxy 100,100
   print "Level ";level
   font 0
  for i = 1 to balls
      putimage imgball,i*15,149
  next i
  return

LoadImages:
  x = 0
  y = 0
  for i = 0 to 7
      fname = path+"\"+imgnames(i)+".IMG"
      images(i) = LoadImage(fname,0)

	  if images(i) = 0 then
	    key =  stringdialogbox (fname+"  Not found",0)
		 halt
	  end if
	  PutImage images(i),x,y
	  x = x + 30
	  if x > 150 then 
	     x = 0
		 y = y + 30
	  end  if
  next i
  return

Sound:
  for i = 1 to 50
    out 108,0
	out 108,255
  next i
  out 108,0
  return

halt
 