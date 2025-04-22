

dim b as byte
dim i as integer
dim l as long
dim f as float
dim x as float
dim key as integer
dim key1 as integer
dim s as string
dim pos as long
dim menu as string
dim fname as string

type TTest
   m_f as float
   m_i as integer
   m_l as long
   m_b as byte
end type


dim pages(100) as long

declare sub WriteNumbers()
declare sub AnyKey()
declare sub WriteNumbers()
declare sub SumByte()
declare sub SumInteger()
declare sub SumFloat()
declare sub SumLong()
declare sub WriteBin()
declare sub ReadBin()


cls
key = 1
while key <> 0
  menu="010File1^020Sequential files^030Block file^040Info^#"
  menu=menu+"100Exit^200Read a file^210Write numbers^"
  menu=menu+"210Summe byte^220Summe integer^"
  menu=menu+"^230Summe long^240Summe float"
  menu=menu+"^300Write structure^310Read Structure^"
  menu=menu+"^400About^"
  key = StringMenu(menu,0)
  cls
  if key = 900 then gosub ReadFile
  if key = 930 then WriteNumbers
  if key = 960 then SumByte
  if key = 990 then SumInteger
  if key = 1020 then SumLong
  if key = 1050 then SumFloat
  print key
wend

halt

halt

ReadFile:
    cls
	dim page as integer
    fname = PickFile("MMCard1","MMCard1","TXT|BAS|DIS","Select a file")
	print "Reading ";fname
	open fname for input as #1
	pages(0) = 0
	page = 1
	cls

ReadLoop:

    if eof(1) = 1 then goto ReadExit
	pos = tell(1)
	input #1,s
	print s
	
	if cursory > 130 then 
	   
	   pages(page) = pos
	   gotoxy 0,147
	   line 1,0,146,239,146
	   print "page ";page;" space = next, up = prev, esc = quit";
	   key = getkey
	   cls
	   if key = 18 then goto ReadExit
	   if key = 3 then
	      if page > 0 then 
		     page = page -1
			 seek 1,pages(page)
		  end if
		  goto ReadLoop
	   end if
	   page = page + 1
	end if
	
	goto ReadLoop

ReadExit:
	close 1
	key = getkey
    return

sub Anykey()
   dim key as integer
   gotoxy 0,147
   print "Press any key";
   key = getkey
end sub

sub WriteNumbers()
   dim i as integer
   open "MMCard1\numbers.txt" for output as #1
   cls
   randomize 1
   for i = 1 to 10
       x = rand(100)
       print #1,x
	   print x
	   
   next i
   close 1
   Anykey
end sub

sub SumByte()
   dim summe as byte
   dim zahl as byte
   dim i1 as byte
   open "MMCard1\numbers.txt" for input as #1
   cls
   print "Sum of numbes in file (byte)"
   summe = 0
   i1 = 0
   zahl = 0
   for i1 = 1 to 10
       input #1,zahl
       print Zahl
	   summe = summe + zahl
   next i1
   print "Summe = ";summe
   AnyKey
   close 1
end sub

sub SumInteger()
   dim summe as integer
   dim zahl as integer
   dim i1 as integer
   open "MMCard1\numbers.txt" for input as #1
   cls
   print "Sum of numbes in file (integer)"
   summe = 0
   i1 = 0
   zahl = 0
   for i1 = 1 to 10
       input #1,zahl
       print Zahl
	   summe = summe + zahl
   next i1
   print "Summe = ";summe
   AnyKey
   close 1
end sub

sub SumLong()
   dim summe as long
   dim zahl as long
   dim i1 as integer
   open "MMCard1\numbers.txt" for input as #1
   cls
   print "Sum of numbes in file (long)"
   summe = 0
   zahl = 0
   i1 = 0
   for i1 = 1 to 10
       input #1,zahl
       print Zahl
	   summe = summe + zahl
   next i1
   print "Summe = ";summe;
   close 1
   Anykey
end sub

sub SumFloat()
   dim summe as float
   dim zahl as float
   dim i as integer
   open "MMCard1\numbers.txt" for input as #1
   cls
   print "Sum of numbes in file (float)"
   summe = 0
   i = 0
   zahl = 0
   for i = 1 to 10
       input #1,zahl
       print Zahl
	   summe = summe + zahl
   next i
   print "Summe = ";summe
   AnyKey
   close 1
end sub

sub WriteBin()
   dim xx as TTest
   xx.m_f = 1.3
   xx.m_l = 100000
   xx.m_i = 1000
   xx.m_b = 100

   open "MMCard1\test.bin" for output as #1
   put #1,xx
   close 1
end sub

sub WriteBin()
   dim xx as TTest
   xx.m_f = 1.3
   xx.m_l = 100000
   xx.m_i = 1000
   xx.m_b = 100

   open "MMCard1\test.bin" for input as #1
   get #1,xx
   close 1
   cls
   print "x.m_b =";xx.m_b
   print "x.m_i =";xx.m_i
   print "x.m_l =";xx.m_l
   print "x.m_f =";xx.m_f
   AnyKey
end sub


halt
 
  