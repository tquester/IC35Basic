
#include <ic35sdk.h>
#include "data.h"

/* #define _msg(x) */
/*
void _msg(U8 *s)
{
	U8 buf[30];
	strcpy(buf," ");
	WriteText(1,buf,120,0);
}
*/

void ic_gotoxy(ICTerminal *term, S16 x, S16 y)
{
	term->curX = x;
	term->curY = y;
}

U8 ic_checkerr(ICTerminal *term)
{
	char buf[15];
	char buf2[60];
	if (term->curX > 239  || term->curY > 159 ) 
	{
		    strcpy(buf2,"icstdio error curX=");
			itoa(term->curX,buf,10); strcat(buf2,buf);
			strcat(buf2," curY=");
			itoa(term->curY,buf,10); strcat(buf2,buf);
			WriteText(1,buf2,0,0);

			GetKey();
			return 0;
	}
	return 0;
}


void ic_newline(ICTerminal *term)
{
	term->curY += term->lineHeight;
	term->curX =  term->x1;
	if (term->curY+term->lineHeight>term->y2) {
		ScrollPageUp(term->x1,term->y1,term->x2,term->y2-1,term->lineHeight-1,term->backColor);
		term->curY -= term->lineHeight;
	}
}




/* Write a string, which does not contain special characters such as \n, \r or \t
*/
void ic_write(ICTerminal *term, U8 *buf)
{
	S16 w,w1,slen,rest,i;
	U8 save, *p;


	if (ic_checkerr(&gTerm)) return;
	w = GetTextWidth(buf);
	slen = strlen(buf);
	while (w > 0)
	{
		if (w+term->curX > term->x2) {
			/* We have too much chars, lets cut the string */
			rest = term->x2 - term->curX;
			slen = strlen(buf);
			w1 = 0;
			for (i=0;i<slen;i++)
			{
				w1 += GetCharWidth(buf[i]);
				if (w1 > rest) {
					i--;
					break;
				}
			}
			if (i < 0) 
				break;
			p = buf+i;
			save = *p;
			*p = 0;
			WriteText(term->color,buf,term->curX,term->curY);
			w-=GetTextWidth(buf);
			*p = save;
			buf = p;

			ic_newline(term);
		}
		else
		{
			WriteText(BLACK,buf,term->curX,term->curY);
			term->curX += w;
			w = 0;
		}

	}
	SetCursorPosition(gTerm.curX,gTerm.curY);
}


void ic_puts(ICTerminal *term, U8 *buf)
{
	U8 c,*s,save,*start;
	U16  tw, w;
	start = buf;
	s = start;

	while ((c=*s++) != 0)
	{
		if (c == '\r' || c == '\n' || c == '\t') {
			s--;
			save =*s;
			*s = 0;
			if (*start != 0)
				ic_write(term,start);
			*s = save;
			s++;
			start = s;
			switch(c)
			{
				case '\n': ic_newline(term);			break;
				case '\r': term->curX = term->x1;		break;
				case '\t': tw = (term->x2 - term->x1) / term->numTabs;
						   w = (term->curX) / tw;
						   term->curX = tw * (w+1);
						   if (term->curX > term->x2) ic_newline(term);
						   break;
			}
			start = s;

		}
	}
	if (*start != 0)
		ic_write(term,start);

}
			


