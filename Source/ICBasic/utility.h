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

#ifndef _UTILITY_H
#define _UTILITY_H

#include <ic35sdk.h>

#define	ZG_A	48271
#define ZG_Div	44488
#define ZG_Mod	3399
#define ZG_M	2147483647

extern void zg_init(S32);
extern void zg_tinit();
extern S32 zg_rand();
extern void Line(U8 x1, U8 y1, U8 x2, U8 y2,U8 Color);

extern void snd_ToneOn(U16);
extern void snd_ToneOff();

extern U32 atol(char *);
extern U16 upcase(U16);


#endif