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

#include <ic35sdk.h>
#include "ic35term.h"

/* Set the upper left and the lower right coordinates of the text window */
void	it_SetWindow(TTWindow *wh,U16 xs,U16 ys,U16 xe,U16 ye)
{
	if (xs>LCD_WIDTH)
	{
		xs=LCD_WIDTH;
	}
	if (xe>LCD_WIDTH)
	{
		xe=LCD_WIDTH;
	}
	if (ys>LCD_HEIGHT)
	{
		ys=LCD_HEIGHT;
	}
	if (ye>LCD_HEIGHT)
	{
		ye=LCD_HEIGHT;
	}
	wh->xs=xs;
	wh->ys=ys;
	wh->xe=xe;
	wh->ye=ye;
}

/* If bw=1, then words are broken at the line ending, otherwise they are put into the next line */
/* if they are not longer than a complete line													*/
void	it_SetBreakWords(TTWindow *wh,U8 bw)
{
	wh->bw=bw;
}

/* Sets the distance between to text lines in pixel												*/
void	it_SetLineDist(TTWindow *wh,S8 ld)
{
	wh->ldist=ld;
	wh->space=wh->height+wh->ldist;
}

/* Set the text cursor in pixels relative to the window coordinates								*/
void	it_SetCursor(TTWindow *wh,U16 x,U16 y)
{
	wh ->cx=wh->xs+x;
	wh ->cy=wh->ys+y;
}

/* Set one of the 6 IC35 fonts to use															*/
void	it_SetFont(TTWindow *wh, U8 fm)
{
	SetFontMode(fm);
	wh->font=fm;
	wh->height=((fm & 7)>=3)?16:12;
	wh->space=wh->height+wh->ldist;
}

/* Determine, if actual drawing operations take place, or if only the line braking is done		*/
/* without drawing anything																		*/
void	it_SetVisible(TTWindow *wh, BOOL visible)
{
	wh->visible=visible;
}

/* Set the text screen to the complete IC35 screen, don't break words, set normal line distance */
/* set standard font, set cursor to upper left corner, set visible display						*/
void it_BasicSetup(TTWindow *wh)
{
	it_SetWindow(wh,0,0,LCD_WIDTH-1,LCD_HEIGHT-1);
	it_SetBreakWords(wh,0);
	it_SetLineDist(wh,0);
	it_SetFont(wh,0);
	it_SetCursor(wh,0,0);
	it_SetVisible(wh,TRUE);
}

/* Clear the windows																			*/
void it_ClrScr(TTWindow *wh)
{
	if (wh->visible)
	{
		FillBlock(WHITE,(short)wh->xs,(short)wh->ys,(short)(wh->xe-wh->xs),(short)(wh->ye-wh->ys));
	}
	wh->cx=wh->xs;
	wh->cy=wh->ys;
}

/* Print one screenline. The text in text is printed as long as the end of the input string	is	*/
/* reached or the next word doesn't fit on the line anymore or there is a newline in the input	*/
/* string. If breakwords is enabled the line													*/
/* is always filled to its end by breaking words, otherwise a word is only broken, if it		*/
/* is longer than one screenline to avoid infinite loops in the calling functions				*/
/* If not the complete input string could be printed on the line, a pointer to the rest of the  */
/* string is returned, otherwise NULL															*/
/* The print is done with the currently selected font											*/
char *it_printsln(TTWindow *wh,char *text)
{
	char *wstart,*wnext;
	char save;
	U16 spwidth;
	U16 width;
	U8 lf;
	spwidth=GetCharWidth(' ');
	wnext=text;
	/* step through all the words */
	while (wnext!=NULL)
	{
		/* memorize the beginning of the current word */
		wstart=wnext;
		/* no linefeed found yet */
		lf=0;
		/* Find the next word delimiter */
		while (*text!=0 && *text!=' ' && *text!='\t' && *text!='.' && *text!=';' && *text!=',' && *text!='!' && *text!='"' && *text!='?' && *text!=':' && *text!='-' && *text!=10 && *text!=13)
		{
			text++;
		}		
		/* Remove the string delimiter from the print string and end procedure */
		if (!(*text))
		{
			text--;
			wnext=NULL;
		}
		else
		{
			wnext=text+1;
			/* Do we have a newline */
			if (*text==10 || *text==13)
			{
				/* If we have a linefeed after our word, we must do a linefeed */
				if (*text==10)
				{
					lf=1;
				}
				/* We can't print the newline-character */
				text--;
			}
		}
		/* Did the previous operation get at least one character ?*/
		if (text>=wstart)
		{
			/* Replace the following character by a 0 to therminate the string for the IC35 routines */
			save=*(text+1);
			*(text+1)=0;
			/* Get the width of the word */
			width=GetTextWidth(wstart);
			/* check, if it fits */
			if (wh->cx+width<=wh->xe)
			{
				if (wh->visible) 
				{
					WriteText(BLACK,wstart,wh->cx,wh->cy);
				}
				wh->cx+=width;
			}
			else
			{
				/* Try, if it fits without the leading space (if there is one) */
				if (*text==' ' && wh->cx+width-spwidth<=wh->xe)
				{
					/* print it without the space */
					*text=0;
					if (wh->visible)
					{
						WriteText(BLACK,wstart,wh->cx,wh->cy);
					}
					wh->cx+=width-spwidth;
					/* restore the space */
					*text=' ';
				}
				else
				{
					/* Check, if the word is longer than one line or if we are in word break mode */
					if (wh->bw || width>=(wh->xe-wh->xs))
					{
						/* Print out as many characters as possible on the current line */
						while((width=GetCharWidth(*wstart)) && wh->cx+width<=wh->xe)
						{
							if (wh->visible)
							{
								WriteChar(BLACK,*wstart,wh->cx,wh->cy);
							}
							wstart++;
							wh->cx+=width;
						}
					}
					/* Now a linefeed has to be done, so return the start address of the first character */
					/* to be printed on the next line													 */
					/* before doing that, we have to restore the character after the current word */
					*(text+1)=save;
					return(wstart);
				}
			}
			/* restore the current words following character */ 
			*(text+1)=save;
		}
		/* the linefeed isn't handled by this routine, so return the rest of the text to be done */
		if (lf)
		{
			return(wnext);
		}
		/* Go to the next word */
		text=wnext;
	}
	/* We have completed all our words, so we can report success */
	return(NULL);
}

/* fill the screen with text: Beginning from the current cursor function the text in the input	*/
/* string is printed on the screen with linebreaks when necessary, until the string ends or the */
/* screen is filled, that no more line fits on it												*/
/* The set font and linedistance are used for this purpose										*/
/* This function returns a pointer to the part of the input string, which hasn't been displayed */
/* so far, or NULL if the complete string could be processed									*/
char *it_printtxt(TTWindow *wh,char *text)
{
	char *nextln;

	nextln=text;
	/* As long we can put lines on the screen, do it */
	while (wh->cy+wh->space<=wh->ye && (nextln=it_printsln(wh,nextln)))
	{
		/* Put the cursor at the beginning of the next line */
		wh->cy+=wh->space;
		wh->cx=wh->xs;
	}
	/* Tell the caller, if there has to be done something, or if everything is finished */
	return(nextln);
}

/* Scroll the screen one line up */
void it_ScrollLine(TTWindow *wh)
{
	/* Do the actual scrolling */
	ScrollPageUp(wh->xs,wh->ys,wh->xe,wh->ye,wh->space,WHITE);
	/* And adjust the cursor */
	wh->cy-=wh->space;
}

/* Scroll the screen, that the last line gets the first */
void it_ScrollScreen(TTWindow *wh)
{
	/* Do the actual scrolling on the screen */
	ScrollPageUp(wh->xs,wh->ys,wh->xe,wh->ye,wh->cy-wh->space,WHITE);
	/* And adjust the cursor */
	wh->cy=wh->ys+wh->space;
}

#ifdef STANDALONE
/* Print a complete text on the screen. If the text is longer than the screen the screen is		*/
/* scrolled																						*/
void it_print(TTWindow *wh,char *text)
{

	/* As long as we have text to print */
	while(text)
	{
		/* print as much as possible */
		text=it_printtxt(wh,text);
		/* Is there enough space for the next line ? */
		if (wh->cy+wh->space>wh->ye)
		{
			/* If not, move the screen one line up */
			it_ScrollLine(wh);
		}
	}
}
#endif
