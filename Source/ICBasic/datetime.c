#include <ic35sdk.h>
#include "icstdioq.h"

_itoa2(U8 *buf, S16 number)
{
	U8 *p = buf;
	if (number < 10) *p++='0';
	itoa(number,p,10);
}

void GetFormatDate(U8 *s)
{
	TIMEDCS  tcs;
	GetTime(&tcs);
	_itoa2(s,tcs.day);
	_itoa2(s+3,tcs.month);
	itoa(tcs.year,s+6,10);
	s[2] = '.';
	s[5] = '.';
	s[10]= 0;
}

void GetFormatTime(U8 *s)
{
	TIMEDCS  tcs;
	GetTime(&tcs);
	_itoa2(s,tcs.hour);
	_itoa2(s+2,tcs.minute);
	_itoa2(s+2,tcs.second);
	s[2] = ':';
	s[5] = ':';
	s[7]= 0;
}
