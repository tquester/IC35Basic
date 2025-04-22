#ifndef __ICSTUDIO__
#define __ICSTUDIO__

typedef struct 
{
	U16 x1,y1,x2,y2;
	U16 curX,curY;
	U8  avCharWidth;
	U8  lineHeight;
	U8  color;
	U8  backColor;
	U8  numTabs;
} ICTerminal;


/*
#define putchar(c)   ic_putchar(&gTerm,c);
#define puts(s)      strcpy(gPrintBuf,s); ic_puts(&gTerm,gPrintBuf);
#define initterm()   InitICTerm(&gTerm,gPrintBuf,255)
#define cls()		 ic_cls(&gTerm)
*/



U8 ic_putchar(ICTerminal *term, U8 c);
void InitICTerm(ICTerminal *term, U8 *buf, U16 size);
void ic_puts(ICTerminal *term, U8 *buf);
void ic_cls(ICTerminal *term);
void ic_newline(ICTerminal *term);
void ic_gotoxy(ICTerminal *term, S16 x, S16 y);
U8 ic_stricmp(U8 *a,U8 *b);
U8 _toupper(U8 c);
void _strncpy(U8 *d, U8 *s, U16 len);
void ltohex(U8 *s, U32 x);
void utohex(U8 *s, U16 x);

void GetFormatDate(U8 *s);
void GetFormatTime(U8 *s);
U8 ic_checkerr(ICTerminal *term);
void HexByte(U8 **p, U8 x);


#endif