/*    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program in the file COPYING; if not, write to the
    Free Software Foundation, Inc.
    59 Temple Place, Suite 330,
    Boston, MA  02111-1307  USA
*/
#define NL 12
#include "utility.h"

extern U16 wFreq;

/* Initialize the random number generator with a 31 Bit value   */
/* This must be done, before zg_rand() can be used		*/
/* Parameters:							*/
/* start: seed for the random generator				*/
/* Return value:						*/
/* None								*/
/* Initialisierung des Zufallsgenerators mit einer 31 Bit Zahl	*/
/* Dies muss vor der ersten Benutzung von zg_rand geschehen	*/
/* Parameter:							*/
/* start: Initialwert für den Zufallsgenerator			*/
/* Rückgabewert:						*/
/* Keine							*/

void zg_init(S32 start)
{
	extern zg_value;
	zg_value=start;
}

/* This function initializes the random number generator with	*/
/* a time and date dependent start value. So a programm will	*/
/* produce different random numbers at each run, which it wouldn't*/
/* with a constant initialisation				*/
/* Parameters:							*/
/* None								*/
/* Return values:						*/
/* None								*/

/* Diese Funktion initialisiert den Zufallsgenerator mit einem	*/
/* Wert, der von der aktuellen Uhrzeit und vom aktuellen Datum	*/
/* abhängt. Damit produziert ein Programm bei jedem Lauf unter-	*/
/* schiedliche Zufallszahlen, was nicht der Fall wäre, wenn man	*/
/* Mit einer Konstante initialiseren würde			*/
/* Parameter:							*/
/* Keine							*/
/* Rückgabewerte:						*/
/* Keine							*/

void zg_tinit()
{
	TIMEDCS stime;
	S32 rstart;
	GetTime(&stime);		/* Calculate a seed for the random generator	*/
	rstart=*((S32 *)&stime) ^ *((S32 *)&stime+1);
	rstart&=2147483647;
	zg_init(rstart);
}

/* Delivers a pseudo random 32 Bit number in the range from	*/
/* 0 to 2^31-1							*/
/* zg_rand(), zg_init() need a global S32 Variable named zg_value*/
/* Parameters:							*/
/* None								*/
/* Return values:						*/
/* A positive integer pseudo random number in the range 0...2^31-1*/
/* Diese Funktion gibt eine pseudo-zufällige Zahl (die Sequenz	*/
/* der generierten Zahlen ist für den selben zg_init-Wert immer	*/
/* die selbe) im Bereich 0...2^31-1 zurück			*/
/* Die Zufallsgeneratorfunktionen zg_init(), zg_rand() benötigen*/
/* eine globale S32 Variable namens zg_value			*/
/* Parameter:							*/
/* Keine							*/
/* Rückgabewert:						*/
/* Eine Pseudozufällige Ganzzahl zwischen 0 und 3°31-1		*/

S32 zg_rand()
{
	extern zg_value;
	zg_value=ZG_A*(zg_value%ZG_Div)-ZG_Mod*(zg_value/ZG_Div);
	if (zg_value<0)
	{
		zg_value+=ZG_M;
	}
	return(zg_value);
}

/* This function plays a tone. The behavior differs much from 	*/
/* the emulation and the IC35. In the emulation it will generate*/
/* a short tone of the given frequency in Hertz. On the IC35	*/
/* it will turn on a tone. Its period is defined by the sum of	*/
/* the lower 6 Bits of both Bytes. This sum is the divider which*/
/* divides a frequency of 230400 Hz. The lowest frequency, which*/
/* can be generated with this is 230400/(63+63)=1828.57 Hz.	*/
/* Since one number determines the time, the speaker is on and 	*/
/* the other the speaker is off, the duty-cycle of the generated*/
/* Waveform can be controlled in a certain range		*/
/* In order to create a tone therefor none of the both dividers	*/
/* can be 0							*/
/* Parameters:							*/
/* frequency: See description above				*/
/* Return values:						*/
/* None								*/
/* Folgende Funktion erzeugt einen Ton. Leider unterscheiden sich*/
/* IC35 und PC an dieser Stelle ziemlic: Der PC spielt einen 	*/
/* kurzen Ton mit der gegebenen Frequenz in Hertz. Der IC35 	*/
/* dagegen schaltet einen Ton ein, der folgende Eigenschaften 	*/
/* hat: Es gibt zwei 6 Bit Zähler, die mit den jeweils unteren	*/
/* 6 Bits der beiden Bytes des übergebenen Werts vorbelegt werden*/
/* Beide zählen mit einer Taktrate von 230400 Hz runter, wenn	*/
/* der eine fertig ist, fängt der andere an und umgekehrt	*/
/* Wenn der eine Zähler aktiv ist, fliesst Strom durch den Laut-*/
/* sprecher, wenn der andere aktiv ist, dann nicht		*/
/* Wenn T1 und T2 die Anfangswerte der beiden Zähler sind, ist	*/
/* die Frequenz des Tones also 230400/(T1+T2). Das Puls-Pause-	*/
/* Verhältniss ist T1/(T1+T2) und muss verschieden von 0 oder	*/
/* 1 sein, um einen Ton zu erzeugen, am besten 0,5 (T1=T2).	*/
/* T1 und T2 sind 6 Bit Zahlen im Bereich von 0 bis 63. Bei 	*/
/* gegebenem T1 und T2 errechnet sich frequency zu 256*T1+T2	*/
/* Die niedrigste mit dieser Schaltung erzeugbare Frequenz ist	*/
/* 230400/(63+63)=1828.57 Hz, die Frequenzauflösung ist auch	*/
/* recht beschränkt						*/
/* Parameter:							*/
/* frequeny: Frequenz gemäß obiger Beschreibung			*/
/* Rückgabewerte: Keine						*/

void snd_ToneOn(U16 frequency)
{
	/* Real frequency procuced by IC35:
	T1=(frequency & 63);
	T2=(frequency>>8 & 63);
	f=230400/(T1+T2)					*/

	wFreq=frequency;

	#ifdef WIN32
	SetTone( wFreq );
	#else
	#asm
	ld		d,17
	ld		hl, (wFreq_)
	push	hl
	pop     bc   // set frequency and beep
	CALL    11
	#endasm
	#endif
}

/* Turns of the tone generated by snd_ToneOn			*/
/* This function has no effect in the Emulator			*/
/* Parameters:							*/
/* None								*/
/* Return values:						*/
/* None								*/
/* Schaltet auf dem IC35 den Ton, der mit snd_ToneOn() 		*/
/* angeschaltet wurde, wieder ab. Keine Wirkung im Emulator	*/
/* Parameter:							*/
/* Keine							*/
/* Rückgabewerte:						*/
/* Keine							*/
void snd_ToneOff()
{
	#ifndef WIN32
	#asm
	ld      a,0  // turn off beep voice
	ld      d,4
	CALL    11
	#endasm
	#endif
}

/* This function is needed by the Line-Function			*/
/* Diese Funktion ist Teil the Line-Funktion			*/
void l_Swap(U8 *x1, U8 *y1, U8 *x2, U8 *y2)
{
	U8 help;
	help=*x2;
	*x2=*x1;
	*x1=help;
	help=*y2;
	*y2=*y1;
	*y1=help;
}

/* This function is needed by the Line-Function			*/
/* Diese Funktion ist Teil the Line-Funktion			*/
void BresenhamX(U8 x1, U8 y1, U8 dx, U8 dy,S8 incY,U8 Color)
{
   S16 dab,incA,incB;

   dab=(dy<<1)-(S16)dx;
   incA=(dy-(S16)dx)<<1;
   incB=dy<<1;

   WriteDot(Color,x1,y1);
   for (;dx>0;dx--)
   {
	   x1++;
	   if (dab<0)
	   {
		   dab+=incB;
	   }
	   else
	   {
		   dab+=incA;
		   y1+=incY;
	   }
	   WriteDot(Color,x1,y1);
   }
}

/* This function is needed by the Line-Function			*/
/* Diese Funktion ist Teil the Line-Funktion			*/
void BresenhamY(U8 x1, U8 y1, U8 dx, S16 dy,S8 incX,U8 Color)
{
   S16 dab,incA,incB;

   dab=(dx<<1)-(S16)dy;
   incA=(dx-(S16)dy)<<1;
   incB=dx<<1;
   WriteDot(Color,x1,y1);
   for (;dy>0;dy--)
   {
	   y1++;
	   if (dab<0)
	   {
		   dab+=incB;
	   }
	   else
	   {
		   dab+=incA;
		   x1+=incX;
	   }
	   WriteDot(Color,x1,y1);
   }
}
/* This function draws a Line between two points		*/
/* be careful, this function does no clipping			*/
/* Parameters:							*/
/* x1,y1: The Coordinate of the start point of the line		*/
/* x2,y2: The Coordinate of the end point of the line		*/
/* Color: The color of the line (0, WHITE: White, 1, BLACK, black)*/
/* Return values: 						*/
/* None								*/

/* Zeichnet eine gerade Linie zwischen zwei beliebigen Punken	*/
/* Achtung: die Funktion überprüft den Berich der Eingabeparameter*/
/* nicht							*/
/* Parameter:							*/
/* x1,y1: Koordinaten des Startpunktes				*/
/* x2,y2: Koordinaten des Endpunktes				*/
/* Color: Farbe der Linie (0, WHITE: weiss, 1, BLACK: schwarz)	*/
void Line(U8 x1, U8 y1, U8 x2, U8 y2, U8 Color)
{
	S16 dx,dy;
	S8 inc;
	dx=(S16)x2-(S16)x1;
	dy=(S16)y2-(S16)y1;
	inc=1;
	/* Absolute value of dx and dy							*/
	if (dx<0)
	{
		dx=-dx;
	}
	if (dy<0)
	{
		dy=-dy;
	}
	/* Slope of the line steeper than 45 degree				*/
	if (dx>dy)
	{
		if (x1>x2)
		{
			l_Swap(&x1,&y1,&x2,&y2);
		}
		if (y2<y1)
		{
			inc=-1;
		}

		BresenhamX(x1,y1,(U8) dx,(U8) dy,inc,Color);
	}
	else
	{
		if (y1>y2)
		{
			l_Swap(&x1,&y1,&x2,&y2);
		}
		if (x2<x1)
		{
			inc=-1;
		}
		BresenhamY(x1,y1,(U8) dx,(U8)dy,inc,Color);
	}
}

/* Very simple routine for converting a character into upcase 	*/
/* Parameters							*/
/* input: one character						*/
/* Return value:						*/
/* The input value, with the letters a...z converted to upper 	*/
/* case								*/

/* Eine sehr einfache Funktion zum Umwandeln eines Zeichens in	*/
/* Großbuchstaben						*/
/* Parameter							*/
/* input: Das zu wandelnde Zeichen				*/
/* Rückgabewert:						*/
/* Das Zeichen, wobei a...z nach A...Z konvertiert sind		*/
U16 upcase(U16 input)
{
	if (input>='a' && input<='z')
	{
		input=input-'a'+'A';
	}
	return(input);
}


// -----------------------------------------------------------------------
// Desc:	Display signed long value
// Input:	*comment ... text which will be displayed in front of debug message
//			lvalue ..... value to be displayed
//			Zeile ...... line number of output, if "-1" -> PopUp Box
// Output:	none
// Return:	none
// Misc:	StringDialogBox displays max. 130 characters
// -----------------------------------------------------------------------

void WriteSignedLong(U8 *comment, S32 lvalue, S16 Zeile)
{

	S32		*plvalue;
	U8		strvalue[11];			// buffer for 10 characters
	U8		output[51];				// buffer for 50 characters

	if (lvalue > 2147483647)		// lvalue exeeds S32 data range?
		strcpy(output,"[debugLong] Error: lvalue exeeds S32 data range!");
	else
	{
		plvalue = &lvalue;
		ltoa(plvalue,&strvalue[0],10);	// correct values only for S32 (not U32)!!

		strcpy(output,comment);
		strcat(output,": [");
		strcat(output,strvalue);
		strcat(output,"]");
	}

	if (Zeile != -1)
		WriteText(BLACK,&output[0],0,Zeile*NL);
	else
		StringDialogBox(output,OK);

}

// -----------------------------------------------------------------------
// Desc:	Display unsigned long value
// Input:	*comment ... text which will be displayed in front of debug message
//			lvalue ..... value to be displayed
//			Zeile ...... line number of output, if "-1" -> PopUp Box
// Output:	none
// Return:	none
// Misc:	StringDialogBox displays max. 130 characters
// -----------------------------------------------------------------------
void WriteUnsignedLong(U8 *comment, U32 *lvalue, S16 Zeile)
{

	U8	output[131];			// buffer for 130 characters
	U8	val[2];					// 1 char buffer to convert one digit after another from lvalue
	U8	sval[11];
	S8	i,j;					// left and right position in string
	U8	x;						// temp var for swap
	U32	conv;

	strcpy(output,"");

	if (strlen(comment) > 110)		// <comment> (110) + ": [" + (16) +"]" = 130
		strncat(output,comment,110);
	else
		strcat(output,comment);
	strcat(output,": [");

	if (*lvalue > 4294967295)		// unsigned long -> max. 4294967295
	{
		strcat(output,"value too big]");
	}

	else if (*lvalue < 0)			// unsigned long -> min. 0
		{
			strcat(output,"value too small]");
		}

		else if (*lvalue == 0)
			{
				strcat(output,"0]");
			}

			else
			{
				conv = *lvalue;
				sval[0]='\x0';
				for (;;)
				{
					if (conv == 0) break;
					val[0] = (U8)(conv % 10) + '0';	// get last digit and convert to char
					val[1] = '\x0';
					strcat(sval,val);				// assemble digits to string -> reversed order!
					conv = conv / 10;				// kill last digit
				}

				i=0;
				j=strlen(sval)-1;
				for (;;)
				{
					if (i==j || i>j) break;			// i=j -> string len 2,4,6,..; i>j -> string len 1,3,5,..
					x = sval[i];
					sval[i] = sval[j];
					sval[j] = x;
					i++;
					j--;
				}
				strcat(output,sval);
				strcat(output,"]");
			}

	if (Zeile != -1)
		WriteText(BLACK,&output[0],0,Zeile*NL);
	else
		StringDialogBox(output,OK);

}
