dim choice as long
dim key as long

Dim l As Long
Dim i As Integer
Dim b As Short
Dim r1 As Float
Dim r2 As Float
dim o1 as string
dim o2 as integer
Dim x As byte
Dim y As byte 
dim d as integer
dim str1 as string
dim str2 as string
dim str3 as string
dim name as string
dim key as integer
dim hasdot as byte
dim faktor as float
dim operator as string
dim op as byte
dim spc as string
dim rocket1 as long

print
test$="dies ist ein Test"
i = 10-5

line 1,2,3,4,5

rocket1 = LoadImage ("MMCard1\ICBasic\ROCKET1.img")

MainLoop:
  gosub Menu
  
  if choice = 0    then Goto finish
  if choice = 900  then Gosub ArithTest
  if choice = 930  then gosub ForTest
  if choice = 960  then gosub StringTest
  if choice = 1020  then gosub Lines
  if choice = 990  then gosub APITest
  goto MainLoop



rem -----------------------------------------------------------
rem -- Menu
rem -----------------------------------------------------------

Menu:

cls
  str1 = "010File^020Tests^#130Exit^"
  str1=str1+"240Aritmetik^270Speed^280String^290API^200Lines^"
  gotoxy 0,50
  print "ICBasic demo applikation V 0.5"
  for i = 1 to 200
     PutImage rocket1,i,100
  next i
  choice = StringMenu(str1,0)
return

Anykey:
  button  "Press any key to return",30,80,1
  key = GetKey
  return

Lines:
  for x = 0 to 120
      line 1,x*2,0,120,150
  next x
  gosub Anykey
  return




rem -----------------------------------------------------------
rem -- StringTest
rem -----------------------------------------------------------

StringTest:

cls

print "Testing string addition and garbage collection"
input "Please enter your name?";name
str3 = "Hallo " + name + " how are you?"

print "str3=";str3
print "Left(str3,5) = ";left(str3,5)
print "Right(str3,5)= ";right(str3,5)
print "Mid(str3,5,3) = ";mid(str3,5,3)
print "Len(str3) = "; len(str3)

print "str(100) = "; str(100)
str3 = "123.34"
print "str3 = ";str3; " val(str3) = ";val(str3)

print 
print "Testing garbage collection"

iloop:
input "How many dots?";d
if d = 0 then print "Must enter a number": goto iloop


str1 = ""
for i = 1 to d
  gotoxy 0,140
  print i;
  str1 = str1 + "."
next i

print i-1;" Dots: ";
print str1

Gosub Anykey

return


rem -----------------------------------------------------------
rem -- Arithemetic test
rem -----------------------------------------------------------

ArithTest:

cls
for r1 = 1 to 25
   print "1/";r1;"=";1/r1
next i

Gosub Anykey
return

rem -----------------------------------------------------------
rem -- Speed test using a for loop
rem -----------------------------------------------------------

ForTest:

Cls
For y = 1 To 80
  For x = 1 To 120
    dot 1, x * 2, y * 2
  Next x
Next y

Gosub Anykey
return


APITest:
  cls
  o1 = ""
  Button "Testing API function",100,30,1
APILoop:
  key = StringPhoneMenu("010Exit^020Dialogs^030Scroll^040Graphics^",0)
  if key=0 then MakeWarnTone: return
  if key=1 then gosub TestDialogs
  if key=2 then gosub TestScroll
  if key=3 then gosub TestGraphic
  goto APILoop

TestDialogs:
  BackupLCD 0,0,239,159
rem     o1 = CheckBox("Option 1^Option 2^Option 3^","101",120,50)
  o2 =  RadioButton("Option 1^Option 2^Option 3^",2,80,50)
  RestoreLCD 0,0,239,159
  print "Checkbox = ";o1; " Radio button = ";o2
  i = StringDialogBox ("String Dialog",0)
  i = StringDialogBox ("String Dialog",1)
  i = StringDialogBox ("String Dialog",2)
  return 

TestScroll:
  cls
  FillBlock 1,100,70,40,40
  InverseBlock 0,0,239,139
  rectangle 1,100,100,80,50,1
  key = getkey
  cls
  for i = 1 to 10
      vline 1,i*15,0,150,1
  next i
  for i = 1 to 10
      hline 1,40+i*15,0,150,1
  next i
  i = getkey
  for i = 1 to 2
      ScrollUp 0,0,240,160,12,0
  next i
  i = getkey
  for i = 1 to 2
      ScrollDn 0,12,240,160,12,0
  next i
  i = getkey
  for i = 1 to 2
      ScrollLeft 12,0,240,160,12,0
  next i
  i = getkey
  for i = 1 to 2
      ScrollRight 0,0,240,160,12,0
  next i
  i = getkey
  
  print "Indi"
  key = getkey
  for i = 1 to 4
    setindion i	* 16
  next i
  key = getkey
  for i = 1 to 4
    setindioff i * 16
  next i
  return
  cls
TestGraphic:
  cls
  
  print "Scrollbar"
  rectangle 1,100,100,80,50,1
  rectangle 1,120,80,80,50,2

  for i = 1 to 1000
     vscrollbar 1000,20,i,225,0,160
  next i
  
  gosub Anykey
  return   
finish:
End
    