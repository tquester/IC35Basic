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

#ifndef _IC35TERM_H_
#define _IC35TERM_H_
#pragma pack(push,ic35term_h,1)

typedef struct 
{
	/* The coordinates of the text box to be used */
	U16 xs,xe,ys,ye;

	/* Flags: it_bw=1 break in words every time, sc do automatical scroll */
	U8 bw;

	/* Distance between two lines in Pixels, can be negative too */
	S8 ldist;

	/* Cursor Position */
	U16 cx,cy;

	/* Current Font	   */
	U8 font;

	/* Font height in Pixels*/
	U8 height;

	/* Line distance = height+ldist */
	S8 space;

	/* Shall the actual drawing operations been done ?*/
	BOOL visible;
} TTWindow;


extern void	it_SetWindow(TTWindow *,U16,U16,U16,U16);
extern void	it_SetBreakWords(TTWindow *,U8);
extern void	it_SetLineDist(TTWindow *,S8);
extern void it_SetCursor(TTWindow *,U16,U16);
extern void	it_SetFont(TTWindow *, U8);
extern void it_ClrScr(TTWindow *);
extern void it_SetVisible(TTWindow *,BOOL);
extern void it_BasicSetup(TTWindow *);

void it_ScrollLine(TTWindow *);
void it_ScrollScreen(TTWindow *);



extern char * it_printsln(TTWindow *,char *);
extern char * it_printtxt(TTWindow *,char *);
#ifdef STANDALONE
extern void it_print(TTWindow *,char *);
#endif
#pragma pack(pop,ic35term_h)

#endif