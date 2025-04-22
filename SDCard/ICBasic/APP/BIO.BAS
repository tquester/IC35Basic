dim heute as string
dim geb as string
dim t as float
dim m as float
dim j as float
dim a as long 
dim b as float
dim e as float
dim c as long 
dim d as float
dim h as long
dim i as float
dim x as integer
dim y as integer
dim x1 as integer
dim y1 as integer
dim key as byte

declare function Code(d as string) as long
cls
rem input "heutiges Datum? (tt.mm.jjjj)";heute
heute = date
loop:
gosub Titel

input "Geburtsdatum (tt.mm.jjjj)";geb

a= code(heute)
b = code(geb)
e = a-b
print "a=";a;" b=";b;" c=";e
key = getkey
cls
gotoxy 0,0
print "p = Physikalische Kurve";
gotoxy 140,0
print "e = Emotionale Kurve";
gotoxy 0,12
print "g = Geistige Kurve";
gotoxy 140,12
print "Start: ";heute
line 1,0,90,238,90

for i = 1 to 30
    line 1,i*8,88,i*8,92
next i

d = 6.28/23


x1 = 0
y1 = float(90+60*sin(e*d))

for i = 1 to 30
   x = 8*i
  
   y = float(90+60*sin((e+i)*d))
   line 1,x1,y1,x,y
   x1 = x
   y1 = y
next i
gotoxy 230,y1
print "p";

d = 6.28/28
x1 = 0
y1 = float(90+60*sin(e*d))
for i = 1 to 30
   x = 8*i
   y = float(90+60*sin((e+i)*d))
   line 1,x1,y1,x,y
   x1 = x
   y1 = y
next i
gotoxy 230,y1
print "e";

d = 6.28/33
x1 = 0
y1 = float(90+60*sin(e*d))
for i = 1 to 30
   x = 8*i
   y = float(90+60*sin((e+i)*d))
   line 1,x1,y1,x,y
   x1 = x
   y1 = y
next i
gotoxy 230,y1
print "g";

button "Esc=Exit andere = nochmal",0,145,0
key = getkey
if key = 18 then goto exit
if key = 'x' then goto exit
cls
goto loop

goto exit

Titel:
  print "Biorythmus"
  print "Stellt ihre drei Leistungskurven"
  print "(physisch, emuotional, geistig) für ";
  print "die nächsten 30 Tage dar."
  print ""
  print "(Umsetzung des Quellcodes von Klaus Rittmeier)"
  print 
  return 
exit:

function code(d as string) as long
  dim t as integer
  dim m as integer
  dim j as integer
  dim h as long
  dim c as long

  t = val(mid(d,1,2))
  m = val(mid(d,4,2))
  j = val(mid(d,7,4))
  h = float(1/(1+m)+0.7)
  h = float(1 / (1+m)+0.7)
  c = float(30.6001*(1+m+12*h))
  c = c + float(365.25 * (j - h)) + t
  code = c 
end  function

end 
 