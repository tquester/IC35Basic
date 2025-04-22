#include <ic35sdk.h>
#include "data.h"
#include "icstdioq.h"

void InitICTerm(ICTerminal *term, U8 *buf, U16 size)
{
	SetFontMode(NORMAL_FONT);
	term->x1 = 0;
	term->y1 = 0;
	term->x2 =  LCD_WIDTH-1;
	term->y2 =  LCD_HEIGHT-1;
	term->curX = 0;
	term->curY = 0;
	term->color = BLACK;
	term->numTabs = 4;
	term->backColor = WHITE;
	term->lineHeight = NORMAL_FONT_HEIGHT;
	term->avCharWidth = 4;
}

U8 ic_putchar(ICTerminal *term, U8 c)
{
	U16 w = GetCharWidth(c);
	if (ic_checkerr(&gTerm)) return 0;
	if (term->curX + w > term->x2) ic_newline(term);
	
	WriteChar(term->color,c,term->curX,term->curY);
	term->curX += w;
	SetCursorPosition(gTerm.curX,gTerm.curY);
	return c;
}

void ic_cls(ICTerminal *term)
{
	FillScreen(term->backColor);
	term->curX = term->x1;
	term->curY = term->y1;
	InitICTerm(term,NULL,0);
}
