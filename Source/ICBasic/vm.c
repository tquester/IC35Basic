
#include "ic35sdk.h"
#include "data.h"
#include "vm.h"
#include "vmapi.h"
#include "icmemory.h"
#include "icstdioq.h"
#include "compiler.h"
#include "utility.h"
#include "imgdata.h"
#include "config.h"

#ifdef _WIN32
#include "crtdbg.h"
#include "math.h"
#else
#include "ic35math.h"
#endif


double		_atof(char *s);
void		ExecuteVM(U8 *adr);
void		InputLine(U8 *buffer, U8 size);
U8			DisassSingleLine();
void		InitVM();
U8			DebugShowLine(U16 line);
void		RunVMDebug();
void		Msg(char *s);


#define CHKMEM

void cmd_Error()
{
   ERROR;
}

#ifdef _WIN32
#include "stdio.h"
void WriteOpCodeMap()
{
	FILE *fp;
	int i;
	char buf[256],*p;
	fp = fopen("opcodemap.txt","w");
	memset(buf,0,100);
	for (i=0;i<255;i++)
	{
		buf[0] =i;
		gCodePtr = buf;
		

		if (gCmdJumpTable[i] != cmd_Error)
		{
			 CreateDisassLine();
			 gTempBuf[33] = 0; 			 
			 p = strchr(gTempBuf+20,'\n');
			 if (p != NULL)	 *p = 0;
			 p = strchr(gTempBuf+20,'\r');
			 if (p != NULL)	 *p = 0;
			 fprintf(fp,"%02x : %s\n",i, gTempBuf+20);
		}
		else
		{
			 fprintf(fp,"%02x : is free\n",i);
		}

	}
	fclose(fp);
}
#endif

static void _print4(char *s)
{
	strcpy(gPrintBuf,s); 
	ic_puts(&gTerm,gPrintBuf);
}

static void _print4i(S16 i)
{
	itoa(i,gPrintBuf,10);
	ic_puts(&gTerm,gPrintBuf);
}


void doerr(char *a, char *b, U8 c)
{
}


U8 _isdigit2(U8 c)
{
	return c >= '0' && c <= '9';
}

float _atof2(char *s)
{
	char c;
	float faktor = 10;
	float n = 0;
	float d = 0;
	while ((c = *s++) != 0)
	{
		if (c == '.') break;
		if (!_isdigit2(c)) break;
		n = 10 * n + c - '0';
	}
	if (c == '.') 
	{
		while ((c = *s++) != 0)
		{
			if (!_isdigit2(c)) break;
			d += (c-'0') / faktor;
			faktor *=10;
		}
		n += d;
	}
	return n;
}

static void _print3(char *s)
{
	strcpy(gPrintBuf,s); 
	ic_puts(&gTerm,gPrintBuf);
}

static void _print3i(U16 i)
{
	itoa(i,gPrintBuf,10);
	ic_puts(&gTerm,gPrintBuf);
}


     
/* ---------------------------------------------------------------------------  */
/* Loading constants															*/
/* ---------------------------------------------------------------------------  */

#define LOADC(typ) 	*(--((typ*)gStackPtr)) = *((typ*)gCodePtr)++


void cmd_Load_C8()
{
	*--gStackPtr = *gCodePtr++;
}

void cmd_Load_C16()
{
	LOADC(U16);
}

void cmd_Load_C32()
{
	LOADC(U32);
}

void cmd_Load_CF()
{
	LOADC(float);
}

/* ---------------------------------------------------------------------------  */
/* Loading variables															*/
/* ---------------------------------------------------------------------------  */

#define LOADV(typ,vtyp)  *(--((typ*)gStackPtr)) = *(typ*)(&gRegisterVP[*((U16*)gCodePtr)++]); 
#define LOADVI(name,typ) void name() { U16 index;\
		index = *((U16*)gStackPtr)++; \
		*(--((typ*)gStackPtr)) = *(typ*)(&gRegisterVP[*((U16*)gCodePtr)++]+index); }

void cmd_Load_V8()
{
  *(--gStackPtr) = *(U8*)(&gRegisterVP[*((U16*)gCodePtr)++]);
}

void cmd_Load_V16()
{				   
	LOADV(U16,m_s16);
}

void cmd_Load_V32()
{
	LOADV(U32,m_s32)
}

void cmd_Load_VF()
{
	LOADV(float,m_float)
}


// the 16 bit operation is much faster if we make it like this */
void cmd_Load_VI16()
{ 
  *((U16*)(gStackPtr-sizeof(U16))) = *(U16*)(&gRegisterVP[*((U16*)gCodePtr)++]+*((U16*)gStackPtr));
}

LOADVI(cmd_Load_VI8,U8) 
/* LOADVI(cmd_Load_VI16,U16) */
LOADVI(cmd_Load_VI32,U32) 
LOADVI(cmd_Load_VIF,float) 


/* ---------------------------------------------------------------------------  */
/* Storing variables															*/
/* ---------------------------------------------------------------------------  */

#define CMD_STOREV(typ,vtyp) *(typ*)(&gRegisterVP[*((U16*)gCodePtr)++]) = *((typ*)gStackPtr)++; 

#define CMD_STOREVI(name,typ)  void name() { typ val;\
	val  =*((typ*)gStackPtr)++;\
	*(typ*)(&gRegisterVP[(*((U16*)gCodePtr)++) +*((U16*)gStackPtr)++]) = val;}


CMD_STOREVI(cmd_Store_VI8,U8)
CMD_STOREVI(cmd_Store_VI16,U16)
CMD_STOREVI(cmd_Store_VI32,U32)
CMD_STOREVI(cmd_Store_VIF,float)

void cmd_Store_V8()
{
	*(U8*)(&gRegisterVP[*((U16*)gCodePtr)++]) = *gStackPtr++;
}

void cmd_Store_V16()
{
   *(U16*)(&gRegisterVP[*((U16*)gCodePtr)++]) = *((U16*)gStackPtr)++;
}

void cmd_Store_V32()
{
   *(U32*)(&gRegisterVP[*((U16*)gCodePtr)++]) = *((U32*)gStackPtr)++;
}

void cmd_Store_VF()
{
	*(float*)(&gRegisterVP[*((U16*)gCodePtr)++]) = *((float*)gStackPtr)++; 
}

#define DUP(typ) typ x; x = *((typ*)gStackPtr); *(--((typ*)gStackPtr)) = x;  
void cmd_Dup_8()
{
#ifdef _WIN32
	*((U8*)(gStackPtr-sizeof(U8))) = *((U8*)gStackPtr);
	gStackPtr-=sizeof(U8);
#else
#asm
; *((U8*)(gStackPtr-sizeof(U8))) = *((U8*)gStackPtr);

	LD	HL,(gStackPtr_)
	LD	E,(HL)
	DEC	HL
	LD	(HL),E
; gStackPtr-=sizeof(U8);
	LD	(gStackPtr_),HL
; 
#endasm
#endif
}

void cmd_Dup_16()
{
#ifdef _WIN32
	*((U16*)(gStackPtr-sizeof(U16))) = *((U16*)gStackPtr);
	gStackPtr-=2;
#else
/* Original:
; *((U16*)(gStackPtr-sizeof(U16))) = *((U16*)gStackPtr);
	LD	HL,(gStackPtr_)
	LD	E,(HL)
	INC	HL
	LD	D,(HL)
	LD	HL,(gStackPtr_)
	DEC	HL
	DEC	HL
	LD	(HL),E
	INC	HL
	LD	(HL),D
; gStackPtr-=2;
	LD	HL,(gStackPtr_)
	DEC	HL
	DEC	HL
	LD	(gStackPtr_),HL
*/

#asm
; *((U16*)(gStackPtr-sizeof(U16))) = *((U16*)gStackPtr);
	LD	HL,(gStackPtr_)
	LD	E,(HL)
	INC	HL
	LD	D,(HL)
	DEC	HL				; hl = gStackptr
	DEC	HL				; -1
	LD	(HL),D
	DEC	HL				; -2
	LD	(HL),E
; gStackPtr-=2;
	LD	(gStackPtr_),HL
#endasm
#endif
}

void cmd_Dup_32()
{
	*(--((U32*)gStackPtr)) = *((U32*)(gStackPtr+sizeof(U32)));

/*	DUP(U32); */
}

void cmd_Dup_F()
{
/*	DUP(float); */
	*(--((float*)gStackPtr)) = *((float*)(gStackPtr+sizeof(float)));
}

void cmd_AddSp()
{
	U8 x;
	x = *gCodePtr++;
	gStackPtr += x;
}

void cmd_Dec()
{
	U8 c;
	U8 *v;
	c = *gCodePtr++;
	switch( c & 0xff-0x03)
	{
		case ADR_V:
			 v = &gRegisterVP[*((U16*)gCodePtr)++];
			 switch(c & 0x03)
			 {
				 case TYP_8:	(*v)--;			break;
				 case TYP_16:	(*((U16*)v))--;	break;
				 case TYP_32:	(*((U32*)v))--;	break;
				 case TYP_F:	(*((float*)v))--;  break;
			 }
			 break;
	}
}

void cmd_Inc()
{
	U8 c;
	U8 *v;
	c = *gCodePtr++;
	switch(c & 0xff-0x03)
	{
		case ADR_V:
			 v = &gRegisterVP[*((U16*)gCodePtr)++];
			 switch(c & 0x03 )
			 {
				 case TYP_8:	(*v)++;			break;
				 case TYP_16:	(*((U16*)v))++;	break;
				 case TYP_32:	(*((U32*)v))++;	break;
				 case TYP_F:	(*((float*)v))++;  break;
			 }
			 break;
	}
}


/* ---------------------------------------------------------------------------  */
/* Change Type on the stack														*/
/* ---------------------------------------------------------------------------  */

#define EXT_TYP2(typ1,typ2)			\
	typ2 x;							\
	x = (typ2)*((typ1*)gStackPtr)++;		\
	*(--((typ2*)gStackPtr)) = x;	

#define EXT_TYP(typ1,typ2) *((typ2*)(gStackPtr+sizeof(typ1)-sizeof(typ2))) = (typ2)*((typ1*)gStackPtr); \
         gStackPtr -= sizeof(typ2)-sizeof(typ1)
   


void cmd_Ext_8_16()
{
	
    EXT_TYP(S8,S16); 
}

void cmd_Ext_8_32()
{
   EXT_TYP(S8,S32);
}

void cmd_Ext_8_F()
{
	EXT_TYP(S8 ,float );
}

	
void cmd_Ext_16_8()
{
	EXT_TYP(S16 ,S8 );
}

	
void cmd_Ext_16_32()
{
	EXT_TYP(S16 ,S32 );
}

	
void cmd_Ext_16_F()
{
	EXT_TYP(S16 ,float );
}

	
void cmd_Ext_32_8()
{
	EXT_TYP(S32 ,S8 );
}

	
void cmd_Ext_32_16()
{
	EXT_TYP(S32 ,S16 );
}

	
void cmd_Ext_32_F()
{
	EXT_TYP(S32 ,float );
}

	
void cmd_Ext_F_8()
{
	EXT_TYP(float ,S8 );
}

	
void cmd_Ext_F_16()
{
	EXT_TYP(float ,S16 );
}

	
void cmd_Ext_F_32()
{
	EXT_TYP(float ,S32 );
}

/* --------------------------------------------------------------------------------
   -- Binary operations                                                          --	
   --------------------------------------------------------------------------------
*/

/* BINOP is the optimized version of BINOP_ALT, I leave the old version here
   beause the new one is not easy to understand... */

#define BINOP_ALT(typ, op) \
   typ a,b;	   				    \
   a = *((typ*)gStackPtr)++;	\
   b = *((typ*)gStackPtr)++;	\
   *(--((typ*)gStackPtr)) = b op a;

#define BINOP(typ,op) \
   *((typ*)&gStackPtr[sizeof(typ)]) = *((typ*)(gStackPtr+sizeof(typ))) op *((typ*)gStackPtr); \
   gStackPtr+=sizeof(typ); 


void cmd_add_8()
{
#ifdef _IC35
#asm
	LD	HL,(gStackPtr_)
	LD	A,(HL)			; l
	INC	HL
	LD	B,(HL)
	ADD	A,B
	LD	(HL),A
	LD	(gStackPtr_),HL
#endasm
#else
   BINOP(U8,+) 
#endif
}

void cmd_add_16()
{   
#ifdef _IC35
#asm
	LD	HL,(gStackPtr_)
	LD	E,(HL)
	INC	HL
	LD	D,(HL)
	PUSH	DE
	INC	HL
	LD	E,(HL)
	INC	HL
	LD	D,(HL)
	POP	HL
	ADD	HL,DE
	EX	DE,HL

	LD	HL,(gStackPtr_)
	INC	HL
	INC	HL
	LD	(HL),E
	INC	HL
	LD	(HL),D
	DEC	HL
	LD	(gStackPtr_),HL
#endasm
#else
	BINOP(S16,+) 
#endif

   
}

void cmd_add_32()
{
   BINOP(S32,+) 
   
}

void cmd_add_f()
{
   BINOP(float,+) 
   
}


void cmd_sub_8()
{
   BINOP(U8,-)
}

void cmd_sub_16()
{
   BINOP(S16,-)
   
}

void cmd_sub_32()
{
   BINOP(S32,-)
   
}

void cmd_sub_f()
{
   BINOP(float,-)
   
}


void cmd_mult_8()
{
   BINOP(U8,*)
   
}

void cmd_mult_16()
{
   BINOP(S16,*)
}

void cmd_mult_32()
{
   BINOP(S32,*)
}

void cmd_mult_f()
{
   BINOP(float,*)
}




void cmd_div_8()
{
   BINOP(U8,/)   
}

void cmd_div_16()
{
   BINOP(S16,/)   
   
}

void cmd_div_32()
{
   BINOP(S32,/)   
   
}



void cmd_div_f()
{
   BINOP(float,/)    
  
}

void cmd_sin_f()
{
	double a,b;
    a = (double)*((float*)gStackPtr)++;	
#ifdef _WIN32
	 b = sin(a);
#else
	 /* sin(a,&b); */
#endif
	*(--((float*)gStackPtr)) = (float)b;
}

void cmd_cos_f()
{
	double a,b;
    a = *((float*)gStackPtr)++;	
#ifdef _WIN32
	 b = cos(a);
#else
	 /* cos(a,&b); */
#endif
	*(--((float*)gStackPtr)) = b;
}

void cmd_tan_f()
{
	double a,b;
    a = *((float*)gStackPtr)++;
#ifdef _WIN32
	 b = tan(a);
#else
	 /* tan(a,&b); */
#endif
	*(--((float*)gStackPtr)) = b;
}

void cmd_rand_f()
{
	S32 a;
	float f;
	a = zg_rand();
	if (a != 0)
	{
		f = (float)a / ((float)65535*(float)65535);

	}
	else
		f = 0;
	PUSH_STACKF(f);
}

void cmd_randomize()
{
	zg_tinit();
}

void cmd_cmp_8()
{

   S8 c;
   c = gStackPtr[0]-gStackPtr[1];
   gFlagLess = c < 0;
   gFlagZero = c == 0;
   gStackPtr+=2;
}

void cmd_cmp_16()
{
   S16 c;
   c = *((S16*)(gStackPtr+0)) - *((S16*)(gStackPtr+2));
   gFlagLess = c < 0;
   gFlagZero = c == 0;
   gStackPtr+=4;
}


void cmd_cmp_32()
{
	
   S16 c;
   c = *((S32*)(gStackPtr+0)) - *((S32*)(gStackPtr+4));
   gFlagLess = c < 0;
   gFlagZero = c == 0;
   gStackPtr+=8;
}  

void cmd_cmp_f()
{
   float a,b,c,*sp;
   a = *((float*)gStackPtr)++;
   b = *((float*)gStackPtr)++;
   c = a-b;
   gFlagLess = a < b;
   gFlagZero = a == b;
}


void cmd_and_8()
{
   BINOP(U8,&) 
}

void cmd_and_16()
{
   BINOP(U16,&) 
   
}

void cmd_and_32()
{
   BINOP(U32,&) 
   
}

void cmd_or_8()
{
   BINOP(U8,|) 
}

void cmd_or_16()
{
   BINOP(U16,|) 
   
}

void cmd_or_32()
{
   BINOP(U32,|) 
   
}

#define IMPLEMENT_ABS(name,typ) void name() { if ( *((typ*)gStackPtr) < 0) *((typ*)gStackPtr) = -*((typ*)gStackPtr); }

IMPLEMENT_ABS(cmd_abs_8,S8)
IMPLEMENT_ABS(cmd_abs_16,S16)
IMPLEMENT_ABS(cmd_abs_32,S32)
IMPLEMENT_ABS(cmd_abs_F,float)

/*
void cmd_abs_16()
{
	if ( *((S16*)gStackPtr) < 0) 
		*((S16*)gStackPtr) = -*((S16*)gStackPtr);
}
*/
/* -----------------------------------------------------------------------------
   Peek, Poke, In and Out
   ----------------------------------------------------------------------------- */


void cmd_peek()
{
#ifdef _IC35
	U8 *adr;
	adr = (U8*)POP_STACK16;
	PUSH_STACK8(*adr);
#endif
}

void cmd_poke()
{
#ifdef _IC35
	U8 val;
	U8 *adr;
	val = POP_STACK8;
	adr = (U8*)POP_STACK16;
	*adr = val;
#endif
}

void cmd_in()
{
#ifdef _IC35
#asm
	LD	HL,(gStackPtr_)
	LD	C,(HL)
	IN  A,(C)
	LD  (HL),A
#endasm
#endif
}

void cmd_out()
{
#ifdef _IC35
#asm
	LD	HL,(gStackPtr_)
	LD	A,(HL)
	INC HL
	LD  C,(HL)
	INC HL
	LD  (gStackPtr_),HL
	OUT (C),A
#endasm
#endif

}

/* -----------------------------------------------------------------------------
   Jumps
   ----------------------------------------------------------------------------- */

void cmd_Call()
{
   PUSH_STACK16(gCodePtr-gCodeBase + 2);		/* Return address */
   gCodePtr = *((U16*)gCodePtr) + gCodeBase;
}

void cmd_RTS()
{
	gCodePtr = gCodeBase + POP_STACK16;
}

void cmd_Jump()
{
   gCodePtr = *((U16*)gCodePtr) + gCodeBase;
}

void cmd_Jump_z()
{
   if (gFlagZero) 
		gCodePtr =  *((U16*)gCodePtr) + gCodeBase;
   else
        gCodePtr = gCodePtr+2;
}

void cmd_Jump_nz()
{
   if (!gFlagZero) 
		gCodePtr = *((U16*)gCodePtr) + gCodeBase;
   else
        gCodePtr = gCodePtr+2;
}

void cmd_Jump_gt()
{
   if (gFlagLess) 
		gCodePtr =  *((U16*)gCodePtr) + gCodeBase;
   else
        gCodePtr = gCodePtr+2;
}

void cmd_Jump_lt()
{
   if (gFlagLess || gFlagZero) 
		gCodePtr =  *((U16*)gCodePtr) + gCodeBase;
   else
        gCodePtr = gCodePtr+2;
}

void cmd_Jump_ge()
{
   if (!gFlagLess || gFlagZero) 
		gCodePtr =  *((U16*)gCodePtr) + gCodeBase;
   else
        gCodePtr = gCodePtr+2;
}

void cmd_Jump_le()
{
   if (gFlagLess || gFlagZero) 
		gCodePtr =  *((U16*)gCodePtr) + gCodeBase;
   else
        gCodePtr = gCodePtr+2;
}

void cmd_LoadF_z()
{
	*--gStackPtr = gFlagZero;
}


void cmd_LoadF_nz()
{
	*--gStackPtr = gFlagZero == 0;
}


void cmd_LoadF_lt()
{
	*--gStackPtr = gFlagLess;
}


void cmd_LoadF_le()
{
	*--gStackPtr = gFlagZero != 0 | gFlagLess;
}


void cmd_LoadF_gt()
{
	*--gStackPtr = gFlagLess == 0;
}


void cmd_LoadF_ge()
{
	*--gStackPtr = gFlagZero | gFlagLess == 0;
}





/* -----------------------------------------------------------------------------
   BASIC String handling
   ----------------------------------------------------------------------------- */



ICString *CreateStackString()
{
	gStackPtr-=sizeof(ICString);
	return (ICString*)gStackPtr;
}
void PushString(ICString *str)
{
	gStackPtr-= sizeof(ICString);
	memcpy(gStackPtr,str,sizeof(ICString));
}

void PushString2(ICString *str,U8 typ)
{
	ICString *s;
	gStackPtr-= sizeof(ICString);
	memcpy(gStackPtr,str,sizeof(ICString));
	s = (ICString*)gStackPtr;
	s->Typ = typ;

}

void PopString(ICString **str)
{
	*str = (ICString*)gStackPtr;
	gStackPtr += sizeof(ICString);
}

U8  CheckString(ICString *str)
{
	if (str->Typ == TYP_STR_C ||
		str->Typ == TYP_STR_V ||
		str->Typ == TYP_STR_D) return TRUE;
	else
		return FALSE;
}

void DisposeString(ICString *str)
{
	if (str->Typ == TYP_STR_D)
	{
		BASIC_DisposeMemory(str->pString);
	}
}

void cmd_Load_STR_C()
{
	U16 len;
	U8 *adr;
	ICString str;

	len = *((U16*)gCodePtr)++;
	adr = (U8*)gCodePtr;
	gCodePtr += len;
	str.Len = len;
	str.Typ = TYP_STR_C;
	str.pString = adr;
	PushString(&str);
//	*(--((U8**)gStackPtr)) = adr;
}

void cmd_Load_STR_V()
{ 
	ICString *var;
	var = (ICString*)(gRegisterVP + *((U16*)gCodePtr)++);
	PushString2(var,TYP_STR_V);
}

void cmd_Load_STR_VI()
{ 
	U16 index;
	ICString *var;
	index = *((U16*)gStackPtr)++; 
	var = (ICString*)(gRegisterVP + index + *((U16*)gCodePtr)++);
	PushString2(var,TYP_STR_V);
}

void cmd_Store_STR_V()
{
	
	ICString *str;
	ICString *var;
	U16 offset;

	PopString(&str);

	offset = *((U16*)gCodePtr)++;
	var = (ICString*)(gRegisterVP + offset);
	if (var->Typ == TYP_STR_V)
		BASIC_DisposeMemory(var->pString);
	CHKMEM

	switch(str->Typ)
	{
		case TYP_STR_C:
			 var->Len		= str->Len;
			 var->pString	= str->pString;
			 var->Typ		= str->Typ;
			 break;
		case TYP_STR_V:
			 var->pString = BASIC_AllocateString(str->Len,&var->pString);
			 CHKMEM
			 if (var->pString == NULL) ERROR;
			 memcpy(var->pString,str->pString,str->Len);
			 CHKMEM
			 var->Typ     = TYP_STR_V;
			 var->Len     = str->Len;
			 break;
		case TYP_STR_D:
			 var->pString = BASIC_AllocateString(str->Len,&var->pString);
			 CHKMEM
			 if (var->pString == NULL) ERROR;
			 memcpy(var->pString,str->pString,str->Len);
			 CHKMEM
			 var->Typ     = TYP_STR_V;
			 var->Len     = str->Len;
			 BASIC_DisposeMemory(str->pString);
			 break;
	}

			 
	CHKMEM
	
	
}

 
void cmd_Store_STR_VI()
{
	ICString *str;
	ICString *var;
	U16 offset,index;

	PopString(&str);
	index = *((U16*)gStackPtr)++;

	offset = *((U16*)gCodePtr)++;
	var = (ICString*)(gRegisterVP + offset + index);
	if (var->Typ == TYP_STR_V)
		BASIC_DisposeMemory(var->pString);
	CHKMEM

	switch(str->Typ)
	{
		case TYP_STR_C:
			 var->Len		= str->Len;
			 var->pString	= str->pString;
			 var->Typ		= str->Typ;
			 break;
		case TYP_STR_V:
			 var->pString = BASIC_AllocateString(str->Len,&var->pString);
			 CHKMEM
			 if (var->pString == NULL) ERROR;
			 memcpy(var->pString,str->pString,str->Len);
			 CHKMEM
			 var->Typ     = TYP_STR_V;
			 var->Len     = str->Len;
			 break;
		case TYP_STR_D:
			 var->pString = BASIC_AllocateString(str->Len,&var->pString);
			 CHKMEM
			 if (var->pString == NULL) ERROR;
			 memcpy(var->pString,str->pString,str->Len);
			 CHKMEM
			 var->Typ     = TYP_STR_V;
			 var->Len     = str->Len;
			 BASIC_DisposeMemory(str->pString);
			 break;
	}
	CHKMEM
}

void cmd_Add_STR()
{
	ICString *str1, *str2, str4,str5;
	ICString *str3;
	char *p1,*p2;
	U16 len;

	PopString(&str1);
	PopString(&str2);
	p1 = str1->pString;
	p2 = str2->pString;
	memcpy(&str4,str2,sizeof(ICString));
	memcpy(&str5,str1,sizeof(ICString));
	str3 = CreateStackString();
	str3->Len = str5.Len+str4.Len-1;
	str3->pString = BASIC_AllocateString(str3->Len,&str3->pString);
	if (str3->pString == NULL)
		ERROR;
	str3->Typ = TYP_STR_D;
	strcpy(str3->pString,str4.pString);
	CHKMEM
	strcat(str3->pString,str5.pString);
	CHKMEM
#ifdef _WIN32
	_ASSERT(strlen(str3->pString)+1 == str3->Len);
#endif
	len = strlen(str3->pString);
	DisposeString(&str5);
	DisposeString(&str4);
	CHKMEM
}



void cmd_date_s()
{
	ICString *str;

	str = CreateStackString();
	str->Len = 10+1;
	str->Typ = TYP_STR_D;
	str->pString = BASIC_AllocateString(str->Len,&str->pString);
	GetFormatDate(str->pString);
}

void cmd_time_s()
{
	ICString *str;
	TIMEDCS  tcs;

	str = CreateStackString();
	str->Len = 10+1;
	str->Typ = TYP_STR_D;
	str->pString = BASIC_AllocateString(str->Len,&str->pString);
	GetFormatTime(str->pString);
}

void cmd_CMP_STR()
{
	U8 *s1, *s2;
	U8 typ1,typ2;
	S16 r;
	ICString *str1,*str2;
	PopString(&str1);
	PopString(&str2);
	r = strcmp(str1->pString,str2->pString);
	if (str1->Typ == TYP_STR_D)		BASIC_DisposeMemory(str1->pString);
	if (str2->Typ == TYP_STR_D)		BASIC_DisposeMemory(str2->pString);
	gFlagZero  = r == 0;
	gFlagLess  = r <  0;
}


void cmd_GetKey()
{
	PUSH_STACK16(GetKey());
}

void cmd_ScanKey()
{
	PUSH_STACK16(ScanKey());
}


void cmd_ASC()
{
	U8 v;
	ICString *str;
	PopString(&str);
	

	v = str->pString[0];
	
	if (str->Typ == TYP_STR_D)
		BASIC_DisposeMemory(str->pString);
	PUSH_STACK16(v);
}

void cmd_LEN()
{
	U8 typ,*s;
	U16 l;
	ICString *str;
	
	PopString(&str);
	typ = str->Typ;
	s = str->pString;
	l = strlen(str->pString);
	PUSH_STACK16(l);
	if (typ == TYP_STR_D)
		BASIC_DisposeMemory(s);
}

void cmd_VAL()
{
	float f;
	ICString *str;
	PopString(&str);
	f = _atof2(str->pString);
	if (str->Typ == TYP_STR_D)
		BASIC_DisposeMemory(str->pString);
	PUSH_STACKF(f);
}


void cmd_CHR()
{
	U8 x = *((U16*)gStackPtr)++;
	ICString *str;
	str = CreateStackString();
	str->pString = BASIC_AllocateString(2,&str->pString);
	str->Len = 2;
	str->Typ = TYP_STR_D;
	str->pString[0] = x;
	str->pString[1] = 0;
}

void cmd_STR()
{
	
	ICString *str;
	float f;
	f = POP_STACKF;
	str = CreateStackString();
	ftoa(f,gPrintBuf);
	str->Len = strlen(gPrintBuf)+1;
	str->pString = BASIC_AllocateString(str->Len,&str->pString);
	str->Typ = TYP_STR_D;
	strcpy(str->pString,gPrintBuf);
}

void cmd_MID()
{
	S16 start,len,slen;
	U8 typ;
	ICString *str1,*str2;
	char *s;

	len = POP_STACK16;
	start = POP_STACK16;

	PopString(&str1);
	typ = str1->Typ;
	s = str1->pString;
	slen = strlen(s);

	str2 = CreateStackString();
	if (len+start-1 > slen) len = slen-start;
	if (start-1 > slen) len = 0;
	
	str2->Len = len+1;
	str2->pString = BASIC_AllocateString(str2->Len,&str2->pString);
	str2->Typ = TYP_STR_D;
	if (len != 0)
		_strncpy(str2->pString,s+start-1,len);
	else
		str2->pString[0] = 0;
	if (typ == TYP_STR_D)
		BASIC_DisposeMemory(s);
}

void cmd_LEFT()
{
	S16 len,slen;
	U8 typ;
	U8 *s;
	ICString *str1,*str2;
	len = POP_STACK16;
	PopString(&str1);
	typ = str1->Typ;
	s = str1->pString;
	slen = strlen(s);
	if (len > slen) len = slen;
	str2 = CreateStackString();
	str2->Typ =TYP_STR_D;
	str2->pString = BASIC_AllocateString(len+1,&str2->pString);
	str2->Len = len+1;
	_strncpy(str2->pString,s,len);
	if (typ == TYP_STR_D)
		BASIC_DisposeMemory(s);
}

void cmd_RIGHT()
{
	S16 len,slen;
	U8 typ;
	U8 *s;
	ICString *str1,*str2;
	len = POP_STACK16;
	PopString(&str1);
	typ = str1->Typ;
	s = str1->pString;
	slen = strlen(s);
	len = slen-len;
	if (len < 0) len = 0;
	if (len > slen) len = slen;
	str2 = CreateStackString();
	str2->Typ =TYP_STR_D;
	str2->pString = BASIC_AllocateString(len+1,&str2->pString);
	str2->Len = len+1;
	strcpy(str2->pString,s+len);
	if (typ == TYP_STR_D)
		BASIC_DisposeMemory(s);
}

/* -----------------------------------------------------------------------------
   BASIC Input and print
   ----------------------------------------------------------------------------- */


void cmd_Input_8()
{
	gTempBuf[0] = 0;
	InputLine(gTempBuf,sizeof(gTempBuf)-1);
	PUSH_STACK8((U8)atoi(gTempBuf));
}


void cmd_Input_16()
{
	gTempBuf[0] = 0;
	InputLine(gTempBuf,sizeof(gTempBuf)-1);
	PUSH_STACK16((U16)atoi(gTempBuf));
}


void cmd_Input_32()
{
	gTempBuf[0] = 0;
	InputLine(gTempBuf,sizeof(gTempBuf)-1);
	PUSH_STACK32((U32)atoi(gTempBuf));
}


void cmd_Input_F()
{
	float x;
	gTempBuf[0] = 0;
	InputLine(gTempBuf,sizeof(gTempBuf)-1);
	x = atoi(gTempBuf);
	PUSH_STACKF(x);
}


void cmd_Input_STR()
{
	ICString *str;
	gTempBuf[0] = 0;
	InputLine(gTempBuf,sizeof(gTempBuf)-1);
	str = CreateStackString();
	str->Typ = TYP_STR_D;
	str->Len = strlen(gTempBuf)+1;
	str->pString = BASIC_AllocateString(str->Len,&str->pString);
	strcpy(str->pString,gTempBuf);
}

void cmd_Print_STR()
{
	USTRLEN len,len1;
	U8 *pStr,c;
	ICString *str;
	PopString(&str);
	CHKMEM

	if (!CheckString(str)) ERROR;
	CHKMEM
	len1 = str->Len;	
	len = strlen(str->pString);
	pStr = str->pString;
	while (len > 0) {
		if (len > 250)
		{
			len1 = 250;
			len -= 250;
			c = pStr[249];
			pStr[249] = 0;
			strcpy(gPrintBuf,pStr);
			pStr[249] = c;
			pStr += 250;
			ic_puts(&gTerm,gPrintBuf);
		} else {
			strcpy(gPrintBuf,pStr);
			ic_puts(&gTerm,gPrintBuf);
			len = 0;
		}
	}
	DisposeString(str);
	CHKMEM
}

void cmd_Print_8()
{
   gPrintBuf[0] = ' ';
   itoa(*gStackPtr++,gPrintBuf+1, 10);		
   ic_puts(&gTerm,gPrintBuf);
}

void cmd_Print_16()
{
   gPrintBuf[0] = ' ';
   itoa(*((U16*)gStackPtr)++,gPrintBuf+1, 10);		
   ic_puts(&gTerm,gPrintBuf);
}

void cmd_Print_32()
{
   S32 x;					
   x = *((U32*)gStackPtr)++;
   gPrintBuf[0] = ' ';
   ltoa(&x,gPrintBuf+1, 10);		
   ic_puts(&gTerm,gPrintBuf);
}

void cmd_Print_F()
{
   gPrintBuf[0] = ' ';
   ftoa(*((float*)gStackPtr)++,gPrintBuf+1);
   ic_puts(&gTerm,gPrintBuf);
}

void cmd_Print_NL()
{
   ic_newline(&gTerm);
}

void cmd_Breakpoint()
{
	switch(*gStackPtr++)
	{
		case 0:  gDebug = 0; break;
		case 1:	 gDebug = 1; break;
		case 2:	 RunVMDebug(); break;
	}
}

void cmd_SourceLine()
{
	U16 line;
	line = *((U16*)gCodePtr)++;
	if (gDebug == 1) 
		if (DebugShowLine(line) != 0) ERROR;
	/* !! Breakpointhandling here */
}
void cmd_Cls()
{
	ic_cls(&gTerm);
}
void cmd_Plot()
{
	WriteDot(gStackPtr[2],
			 gStackPtr[1],
			 gStackPtr[0]);
	gStackPtr+=3;
}

void cmd_Line()
{
	Line(gStackPtr[6],
		 gStackPtr[4],
		 gStackPtr[2],
		 gStackPtr[0],
		 gStackPtr[8]);
	gStackPtr+=9;
}


void cmd_Gotoxy()
{
	gTerm.curY = *gStackPtr++;
	gTerm.curX = *gStackPtr++;
}


#ifdef _WIN32
void SetCmd(U16 nr, void *cmd)
{
   gCmdJumpTable[nr] = cmd;
}
#else
#define SetCmd(x,y) gCmdJumpTable[x] = y
#endif


void InitVM()
{
   U16 i;
   
   gStackPtr = gStack+STACKSIZE;
   for (i=0;i<=255;i++)
   {
      gCmdJumpTable[i] = cmd_Error;
   }

    SetCmd(CMD_LOAD_C8	,cmd_Load_C8);
    SetCmd(CMD_LOAD_C16	,cmd_Load_C16);
    SetCmd(CMD_LOAD_C32	,cmd_Load_C32);
    SetCmd(CMD_LOAD_CF	,cmd_Load_CF);

    SetCmd(CMD_LOAD_V8	,cmd_Load_V8);
    SetCmd(CMD_LOAD_V16,cmd_Load_V16);
    SetCmd(CMD_LOAD_V32,cmd_Load_V32);
    SetCmd(CMD_LOAD_VF	,cmd_Load_VF);

    SetCmd(CMD_LOAD_V8	+ ADR_INDEX,cmd_Load_VI8);
    SetCmd(CMD_LOAD_V16	+ ADR_INDEX,cmd_Load_VI16);
    SetCmd(CMD_LOAD_V32	+ ADR_INDEX,cmd_Load_VI32);
    SetCmd(CMD_LOAD_VF	+ ADR_INDEX,cmd_Load_VIF);


    SetCmd(CMD_STORE_V8	,cmd_Store_V8);
    SetCmd(CMD_STORE_V16,cmd_Store_V16);
    SetCmd(CMD_STORE_V32,cmd_Store_V32);
    SetCmd(CMD_STORE_VF	,cmd_Store_VF);

    SetCmd(CMD_STORE_V8	 + ADR_INDEX,cmd_Store_VI8);
    SetCmd(CMD_STORE_V16  + ADR_INDEX,cmd_Store_VI16);
    SetCmd(CMD_STORE_V32 + ADR_INDEX,cmd_Store_VI32);
    SetCmd(CMD_STORE_VF	 + ADR_INDEX,cmd_Store_VIF);

	SetCmd(CMD_EXT_8_16		,cmd_Ext_8_16);
	SetCmd(CMD_EXT_8_32		,cmd_Ext_8_32);
	SetCmd(CMD_EXT_8_F		,cmd_Ext_8_F);
	SetCmd(CMD_EXT_16_8 		,cmd_Ext_16_8);
	SetCmd(CMD_EXT_16_32		,cmd_Ext_16_32);
	SetCmd(CMD_EXT_16_F		,cmd_Ext_16_F);
	SetCmd(CMD_EXT_32_8		,cmd_Ext_32_8);
	SetCmd(CMD_EXT_32_16		,cmd_Ext_32_16);
	SetCmd(CMD_EXT_32_F		,cmd_Ext_32_F);
	SetCmd(CMD_EXT_F_8		,cmd_Ext_F_8);
	SetCmd(CMD_EXT_F_16		,cmd_Ext_F_16);
	SetCmd(CMD_EXT_F_32		,cmd_Ext_F_32);

	SetCmd(CMD_ADD_8	,cmd_add_8);
	SetCmd(CMD_ADD_16	,cmd_add_16);
	SetCmd(CMD_ADD_32	,cmd_add_32);
	SetCmd(CMD_ADD_F	,cmd_add_f);

	SetCmd(CMD_SUB_8	,cmd_sub_8);
	SetCmd(CMD_SUB_16	,cmd_sub_16);
	SetCmd(CMD_SUB_32	,cmd_sub_32);
	SetCmd(CMD_SUB_F	,cmd_sub_f);

	SetCmd(CMD_MULT_8	,cmd_mult_8);
	SetCmd(CMD_MULT_16	,cmd_mult_16);
	SetCmd(CMD_MULT_32	,cmd_mult_32);
	SetCmd(CMD_MULT_F	,cmd_mult_f);

	SetCmd(CMD_DIV_8	,cmd_div_8);
	SetCmd(CMD_DIV_16	,cmd_div_16);
	SetCmd(CMD_DIV_32	,cmd_div_32);
	SetCmd(CMD_DIV_F	,cmd_div_f);

	SetCmd(CMD_CMP_8	,cmd_cmp_8);
	SetCmd(CMD_CMP_16	,cmd_cmp_16);
	SetCmd(CMD_CMP_32	,cmd_cmp_32);
	SetCmd(CMD_CMP_F	,cmd_cmp_f);

	SetCmd(CMD_AND_8	,cmd_and_8);
	SetCmd(CMD_AND_16	,cmd_and_16);
	SetCmd(CMD_AND_32	,cmd_and_32);
	SetCmd(CMD_OR_8	,cmd_or_8);
	SetCmd(CMD_OR_16	,cmd_or_16);
	SetCmd(CMD_OR_32	,cmd_or_32);


    SetCmd(CMD_PRINT_8  ,cmd_Print_8);
    SetCmd(CMD_PRINT_16 ,cmd_Print_16);
    SetCmd(CMD_PRINT_32 ,cmd_Print_32);
    SetCmd(CMD_PRINT_F  ,cmd_Print_F);
	SetCmd(CMD_PRINT_NL, cmd_Print_NL);
	SetCmd(CMD_INPUTI + TYP_8,	cmd_Input_8);
	SetCmd(CMD_INPUTI + TYP_16, cmd_Input_16);
	SetCmd(CMD_INPUTI + TYP_32, cmd_Input_32);
	SetCmd(CMD_INPUTI + TYP_F,	cmd_Input_F);
	SetCmd(CMD_INPUT_STR,		cmd_Input_STR);
	SetCmd(CMD_GETKEY,			cmd_GetKey);
	SetCmd(CMD_SCANKEY,			cmd_ScanKey);

	SetCmd(CMD_JUMP		,cmd_Jump	);
	SetCmd(CMD_JUMP_Z	,cmd_Jump_z);
	SetCmd(CMD_JUMP_NZ	,cmd_Jump_nz);
	SetCmd(CMD_JUMP_GT	,cmd_Jump_gt);
	SetCmd(CMD_JUMP_GE	,cmd_Jump_ge);
	SetCmd(CMD_JUMP_LT	,cmd_Jump_lt);
	SetCmd(CMD_JUMP_LE	,cmd_Jump_le);
	SetCmd(CMD_CALL		,cmd_Call);
	SetCmd(CMD_RTS		,cmd_RTS);

	SetCmd(CMD_DUP + TYP_8,		cmd_Dup_8);
	SetCmd(CMD_DUP + TYP_16,	cmd_Dup_16);	
	SetCmd(CMD_DUP + TYP_32,	cmd_Dup_32);
	SetCmd(CMD_DUP + TYP_F,		cmd_Dup_F);  
	SetCmd(CMD_ADDSP,           cmd_AddSp);

	SetCmd(CMD_PLOT,			cmd_Plot);
	SetCmd(CMD_LINE,			cmd_Line);
	SetCmd(CMD_GOTOXY,			cmd_Gotoxy);
	SetCmd(CMD_CLS,				cmd_Cls);
	SetCmd(CMD_SOURCELINE,		cmd_SourceLine);
	SetCmd(CMD_LOAD_STR_C,		cmd_Load_STR_C);
	SetCmd(CMD_PRINT_STR,		cmd_Print_STR);
	SetCmd(CMD_LOAD_STR_V,		cmd_Load_STR_V);
	SetCmd(CMD_STORE_STR_V,		cmd_Store_STR_V);
	SetCmd(CMD_LOAD_STR_VI,		cmd_Load_STR_VI);
	SetCmd(CMD_STORE_STR_VI,	cmd_Store_STR_VI);
	SetCmd(CMD_STRINGADD,		cmd_Add_STR);
	SetCmd(CMD_STRINGCMP,		cmd_CMP_STR);
	SetCmd(CMD_GETKEY,			cmd_GetKey);
	SetCmd(CMD_ASC,				cmd_ASC);
	SetCmd(CMD_CHR,				cmd_CHR);
	SetCmd(CMD_API,				cmd_APICall);
	SetCmd(CMD_LOADF_Z,			cmd_LoadF_z);
	SetCmd(CMD_LOADF_NZ,		cmd_LoadF_nz);
	SetCmd(CMD_LOADF_LT,		cmd_LoadF_lt);
	SetCmd(CMD_LOADF_LE,		cmd_LoadF_le);
	SetCmd(CMD_LOADF_GT,		cmd_LoadF_gt);
	SetCmd(CMD_LOADF_GE,		cmd_LoadF_ge);
	SetCmd(CMD_INC,				cmd_Inc);
	SetCmd(CMD_DEC,				cmd_Dec);
	SetCmd(CMD_SIN,				cmd_sin_f);
	SetCmd(CMD_COS,				cmd_cos_f);
	SetCmd(CMD_TAN,				cmd_tan_f);
	SetCmd(CMD_DATE_S,			cmd_date_s);
	SetCmd(CMD_TIME_S,			cmd_time_s);
	SetCmd(CMD_LEN,				cmd_LEN);
	SetCmd(CMD_ASC,				cmd_ASC);
	SetCmd(CMD_CHR,				cmd_CHR);
	SetCmd(CMD_VAL,				cmd_VAL);
	SetCmd(CMD_STR,				cmd_STR);
	SetCmd(CMD_MID,				cmd_MID);
	SetCmd(CMD_LEFT,			cmd_LEFT);
	SetCmd(CMD_RIGHT,			cmd_RIGHT);
	SetCmd(CMD_RAND_F,			cmd_rand_f);
	SetCmd(CMD_RANDOMIZE,		cmd_randomize);
	SetCmd(CMD_BREAKPOINT,		cmd_Breakpoint);
	SetCmd(CMD_ABS_8,			cmd_abs_8);
	SetCmd(CMD_ABS_16,			cmd_abs_16);
	SetCmd(CMD_ABS_32,			cmd_abs_32);
	SetCmd(CMD_ABS_F,			cmd_abs_F);
	SetCmd(CMD_PEEK,			cmd_peek);
	SetCmd(CMD_POKE,			cmd_poke);
	SetCmd(CMD_IN,				cmd_in);
	SetCmd(CMD_OUT,				cmd_out);

#ifdef _WIN32
	WriteOpCodeMap();
#endif

	InitVMAPI();

}



void InitVM2(U8 *adr)
{
	S16 OldBank;
	CodeHeader *codeHeader;
	_print4("Init VM2\n");
	_print4("Reading code header\n");

	codeHeader = (CodeHeader*)adr;
    gCodeBase   = adr;
	adr+=sizeof(CodeHeader);
	gRegisterVP = (U8*)gVariables;
	_print4("Code size="); _print4i(codeHeader->m_SizeCode);
	_print4("\nVariable size="); _print4i(codeHeader->m_SizeVariables);
	_print4("\n");

/*	gRegisterVP = gCode+codeHeader->m_SizeCode; */
/*	gHeap       = gRegisterVP + codeHeader->m_SizeVariables; */
	gHeapTop    = gHeap;
	gHeapLast	= NULL;
	_print4("Resetting image heap\n");
	
	
	OldBank = GetRamBank();
	SetRamBank(BANK_IMAGES);
	gImagePtr   = gImageData+1;
	SetRamBank(STD_BANK);


	_print4("Setting stack\n");
	gStackPtr   = gStack +gSettings.Size_Stack;
	gHeapCount  = 0;
	gErrorCode  = 0;
	gCodePtr    = adr;
	_print4("Clearing variables\n");
	memset(gVariables,0,gSettings.Size_Variables-1);
	_print4("Starting code\n");
}

void ExitVM2()
{
	if (gErrorCode == ERR_OUT_OF_MEMORY) _print3("\nOut of memory");
	_print3("\nExecution finished at position ");
	_print3i(gCodePtr-gCodeBase);
	_print3("\n");
}

void ExecuteVM(U8 *adr)
{

	U8 *oldCodePtr;
	InitVM2(adr);
#ifdef _WIN32
	while (gCodePtr != NULL)
	{
	   if (gStackPtr-gStack < 1000)
	   {
		      BTN("Stack out of range!");
			  break;
	   }
	   if (gCodePtr < gCode ||
		   gCodePtr > gCode+((CodeHeader*)adr)->m_SizeCode) {
		      BTN("Code out of range!");
			  break;
	   }
	   oldCodePtr = gCodePtr;
	   gCmdJumpTable[*gCodePtr++]();

	   if (gCodePtr == NULL) 
		   break;

	   if (gCodePtr < gCode ||
		   gCodePtr > gCode+((CodeHeader*)adr)->m_SizeCode) {
		      BTN("Code out of range!");
			  break;
	   }
	   oldCodePtr = gCodePtr;
	}
#else
#asm
; while (gCodePtr != ((void *)0))
_ExecuteVMLoop:
	LD	HL,(gCodePtr_)
	LD	A,H
	OR	L
	JR	Z,_ExecuteVMExit
; {
; gCmdJumpTable[*gCodePtr++]();
	LD	E,(HL)
	INC	HL
	LD	(gCodePtr_),HL
	LD	D,0
	EX	DE,HL
	ADD	HL,HL
	LD	DE,gCmdJumpTable_
	ADD	HL,DE
	LD	E,(HL)
	INC	HL
	LD	D,(HL)
	EX	DE,HL
	LD  DE,_ExecuteVMLoop
	PUSH DE
	JP (HL)
; }
	JR	_ExecuteVMLoop
_ExecuteVMExit:
#endasm
#endif 
	gCodePtr = gLastPC;
	ExitVM2();
	
	
}

void RunVMDebug()
{
	U8 opcode;
    VMCOMMAND *cmd;
    while (gCodePtr != NULL)
	{
	   if (DisassSingleLine() == 1) break;
	   opcode = *gCodePtr++;
	   cmd = gCmdJumpTable[opcode];
	   cmd();
	}
	gCodePtr = gLastPC;

}


void ExecuteVMDebug(U8 *adr)
{
    VMCOMMAND *cmd;
	U8         opcode;


	InitVM2(adr);
	RunVMDebug();

	ExitVM2();
}

