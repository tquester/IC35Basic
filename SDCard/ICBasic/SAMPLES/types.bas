
type TestRecord
  Nummer as integer
  PLZ as Integer
  Name as String
  Ort as String
  Strasse as String
  Contact as ContactRecord
end type

type ContactRecord
  Telefon as string
  Email as String
end type

dim key as integer

declare sub Test(a as integer, b as integer, str1 as string)
declare sub Test2(ByVal rec as TestRecord)


dim Rec as TestRecord
dim Rec2(10) as TestRecord
dim I as integer
dim key as integer

cls

Test 1+2,1+2*3,"test"

rec.Nummer = 1
rec.Contact.Telefon = "040 98766856"
rec.Contact.Email = "tquester@outlookfolders.com"
rec.Name = "Thomas"
rec.Ort = "Hamburg"
rec.PLZ = 22309
rec.Strasse = "Prechtsweg 16"


print "Nummer: ";rec.Nummer
print "Name:";rec.name
print "Ort:";rec.ort
print "Strasse:";rec.Strasse
print "Telefon:";rec.contact.telefon
print "Email:";rec.contact.email


key = getkey
for i = 1 to 5
  Rec2(i).Nummer = i
  Rec2(i).PLZ = 20000 + i * 100
 rec2(i).Name   = "Name" + str(i)
next i

for i = 1 to 5
  print "Nr = ";rec2(i).Nummer;
  print "   PLZ =";rec2(i).PLZ;
  print "   Name=";rec2(i).name
next i

halt


sub Test(a as integer, b as integer, str1 as string)
   dim x as integer
   dim y as integer
   dim ix as integer
   dim str2 as string

   str2 = str1

   print "a=";a
   print "b=";b
   
   print "str1=";str1
   print "str2=";str2
   key = getkey
   for ix = 1 to 100
       x = ix * 2
	   y = ix
	   dot 1,x,y
   next i
   key = getkey
end sub




halt
  

 