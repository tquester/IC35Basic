#include <ic35sdk.h>
#include "data.h"

U8 _toupper(U8 c)
{
	if (c >= 'a' && c <= 'z') return c + 'A'-'a';
	return c;
}

U8 ic_stricmp(U8 *a,U8 *b)
{
	U8 r;
	while (TRUE)
	{
		if (*a == 0)
		{
			if (*b == 0) return 0;
			return *b;
		}
		r = _toupper(*a++) - _toupper(*b++);
		if (r != 0) return r;
	}
}

void _strncpy(U8 *d, U8 *s, U16 len)
{
	int i;
	for (i=0;i<len;i++)
		if ((*d++=*s++) == 0) break;
	*d = 0;
}


U8 Nibble(U8 x)
{
	return x < 10? x+'0' : x+'A'-10;
}

void HexByte(U8 **p, U8 x)
{
	U8 *s = *p;
	*s++ = Nibble((U8)(x >> 4));
	*s++ = Nibble((U8)(x & 0x0f));
	*p = s;
}

void utohex(U8 *s, U16 x)
{
	HexByte(&s,(U8)(x >> 8));
	HexByte(&s,(U8)(x & 0xff));
	*s++ = 0;
}	

void ltohex(U8 *s, U32 x)
{
	U32 a,b;
	b = 16;
	a = x >> b;
	HexByte(&s,(U8)(a >> 8));
	HexByte(&s,(U8)(a & 0xff));
	a = x & 0xffff;
	HexByte(&s,(U8)(a >> 8));
	HexByte(&s,(U8)(a & 0xff));
	*s++=0;
}
