rem Image editor 

option explicit

declare sub DrawScreen()
declare sub SetCursor(x as byte, y as byte)
declare sub InitGrid()
declare sub ShowCell(x as byte, y as byte)
declare Sub SetSize()


type TVector
   x as byte
   y as byte
end type

type TSettings
   gridw as byte
   gridh as byte
   cellw as byte
   cellh as byte
end type

dim titel$ as string
dim grid(40,40) as byte
dim mini as TVector
dim maxi as TVector
dim cellw as byte
dim key as integer
dim key1 as integer
dim cx as byte
dim cy as byte
dim settings as TSettings
dim str1 as string
dim filename as string
dim w as integer
dim image as long
dim gx as integer
dim gy as integer
dim savimg as long

filename = "MMCard1"
settings.gridw = 16
settings.gridh = 16
settings.cellw = 8
settings.cellh = 8
mini.x = 180
mini.y = 96
maxi.x = 0
maxi.y = 18
cx = 1
cy = 1

savimg = allocimage(100);
InitGrid
DrawScreen
rem breakpoint 2
Setcursor 1,1
loop:
   gotoxy 0,147
   print "x=";cx;" y=";cy;"         ";
   SetCursor cx,cy
   key = getkey
   SetCursor cx,cy
   if key = 32 then
       CursorStatus 0
       if grid(cx,cy) = 0 then
           grid(cx,cy) = 1
       else 
           grid(cx,cy)=0
       end if
       ShowCell cx,cy
	   SetCursor cx,cy
	   CursorStatus 1
	   goto loop
   end if
   if key = 18 then halt
   if key = 48 then
       cls
       DrawScreen
   end if
   if key = 1 then
      if cx ›0 then cx = cx - 1
   end if
   if key = 2 then
      if cx ‹ settings.gridw then cx = cx + 1
   end if
   if key = 3 then
     if cy › 0 then cy = cy - 1
   end if
   if key = 4 then
      if cy ‹ settings.gridh then cy = cy + 1
   end if
   if key = 12 then goto Menu
 goto loop
     
Menu:
   str1 = "010File^020Edit^#"
   str1 = str1 + "100New^110Open^120Save^"
   str1 = str1 + "130Quit^200Grid size^"
   key = StringMenu(str1,0)
	
   if key =   0 then Gosub GridNew
   if key = 900 then gosub GridSize
   if key =  30 then gosub OpenFile
   if key =  60 then gosub SaveFile
   if key =  90 then halt
   DrawScreen  
  goto loop 

GridNew:
  if StringDialogBox("Are you sure?",0) = 2 then 
     InitGrid
	 DrawScreen
  end if
  return

GridSize:
  rem text,variable,typ,len,x,y,w
  InputBox "Size X",settings.gridw,0,10,10,30,70
  InputBox "Size Y",settings.gridh,0,10,10,30,70
  SetSize
  
  return

OpenFile:
  titel$="Select a file to load"
  filename = PickFile("MMCard1",FileName,"IMG",titel$)
  image = LoadImage(filename,0)
  if image <> 0 then
     PutImage image,mini.x,mini.y
	 InitGrid
	 for gx = 0 to imagew(image)
	    for gy = 0 to imageh(image)
		   grid(gx,gy) = dot(mini.x+gx,mini.y+gy)
		next gy
  next gx
  settings.gridw = imagew(image)
  settings.gridh = imageh(image)
  SetSize
  end if
  return

SaveFile:
    
   GetImage savimg,mini.x,mini.y,settings.gridw,settings.gridh
   PutImage savimg,mini.x,0
   
   InputBox "Filename",filename,0,40,10,30,180   
   SaveImage filename,savimg
   return
halt





  
sub SetCursor(x as byte, y as byte)
    x = maxi.x +( x-1) * settings.cellw
    y = maxi.y + (y-1) * settings.cellh
    cursorshape settings.cellw,settings.cellh
	CursorStatus 1
    gotoxy x,y
end sub

sub ShowCell(x as byte, y as byte)
  dim x1 as byte
  dim y1 as byte
   x1 = maxi.x +( x-1) * settings.cellw
   y1 = maxi.y + (y-1) * settings.cellh
   fillBlock grid(x,y),x1+1,y1+1,settings.cellw-2,settings.cellh-2
   dot grid(x,y),mini.x+x,mini.y+y
end sub


sub DrawScreen()
   cls
   dim ix as byte
   dim iy as byte
   dim x  as integer
   dim y  as integer
   dim w as byte
   dim h as byte

   gotoxy 0,0
   font 4
   print "Imageeditor"
   line 1,0,14,239,14
   font 0

   x = maxi.x
   w = settings.gridw+3
   h = settings.gridh+3
   Rectangle 1,mini.x-1,mini.y-1,w,h,0

  for ix = 1 to settings.gridw
       y = maxi.y
       for iy = 1 to settings.gridh
	       FillBlock grid(ix,iy),x,y,settings.cellw,settings.cellh
           Rectangle 1,x,y,settings.cellw,settings.cellh,0
           dot grid(ix,iy),ix+mini.x,iy+mini.y
		   y = y + settings.cellh
	   next iy
	   x = x + settings.cellw
   next ix


end sub

Sub InitGrid()
   dim ix as byte
   dim iy as byte
   for ix = 1 to 39
      for iy = 1 to 39
        grid(ix,iy)= 0
      next iy
   next ix
end sub 

Sub SetSize()
  if settings.gridw > 40 then settings.gridw = 40
  if settings.gridh > 40 then settings.gridh = 40
  gx = 200 / settings.gridw
  gy = 140 / settings.gridh
  if gx > 8 then gx = 8
  if gy > 8 then gy = 8
  if gx > gy then gx = gy
   
  settings.cellw = gx
  settings.cellh = gx
end sub
 
