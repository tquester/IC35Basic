
#include "ic35sdk.h"
#include "data.h"
#include "vm.h"
#include "icmemory.h"
#include "icstdioq.h"
#include "compiler.h"
#include "vmapi.h"
#include "imgdata.h"

#ifdef _WIN32
void SetCmdAPI(U16 nr, void *cmd)
{
   gAPICmdJumpTable[nr] = cmd;
}
#else
#define SetCmdAPI(x,y) gAPICmdJumpTable[x] = y
#endif

void cmd_API_LoadImage()
{
	S16 OldBank;
	U16 r;
	U32 ptr;
	U16 read;
	char fname[100];
	FILE_IDEN iden;
	ICString *str;

	ptr = 0;
	PopString(&str);
	strcpy(fname,str->pString);
	if (str->Typ == TYP_STR_D) BASIC_DisposeMemory(str->pString);
 
	SetRamBank(BANK_IMAGES);
	if (gImagePtr == NULL)
		gImagePtr = gImageData+1;
	r = mOpenFile(fname,OPEN_EXISTING,&iden);
	if (r == MMC_ERROR_SUCCESS)
	{
		ptr = gImagePtr-gImageData;
		*((U16*)gImagePtr) = iden.dwFileSize;
		gImagePtr += sizeof(U16);
		r = mReadFromFile(&iden,gImagePtr,(U16)iden.dwFileSize,&read);
		if (r == MMC_ERROR_SUCCESS)
		{		
			gImagePtr += iden.dwFileSize;
		}
		mCloseFile(&iden);
	}
	SetRamBank(STD_BANK);

	PUSH_STACK32(ptr);
	
}

void cmd_API_AllocImage()
{
	S16 OldBank;
	U16 size;
	U32 ptr;

	size = POP_STACK16;
	OldBank = GetRamBank();
	SetRamBank(BANK_IMAGES);
	if (gImagePtr == NULL)
		gImagePtr = gImageData+1;
	ptr = gImagePtr-gImageData;
	*((U16*)gImagePtr) = size;
	gImagePtr += size + sizeof(U16);
	SetRamBank(STD_BANK);
	PUSH_STACK32(ptr);
}

void cmd_API_PutImage()
{
	U32 ptr;
	U8  x,y;
	S16 OldBank;

	y = POP_STACK8;
	x = POP_STACK8;
	ptr = POP_STACK32;
	
	OldBank = GetRamBank();
	SetRamBank(BANK_IMAGES);
	if (ptr != NULL && x >= 0 && x <= LCD_WIDTH && y >= 0 && y <= LCD_HEIGHT)
	{
		/*
		if (ptr >= gImageData && ptr <= gImageData + 0x4000)
		{
		*/
			PutImage(gImageData+ptr+sizeof(U16),x,y);
		/* }
		*/
	}
	SetRamBank(STD_BANK);
}

void cmd_API_GetImage()
{
	U32 ptr;
	U8  x,y,w,h,b;
	S16 OldBank,size;
	U8 *data;

	h = POP_STACK8;
	w = POP_STACK8;
	y = POP_STACK8;
	x = POP_STACK8;
	b = y+h;
	ptr = POP_STACK32;
	/*
	y -= (y & 7);
	h -= (h & 7);
	while (y+h < b)
		h += 8;
	*/
	OldBank = GetRamBank();
	SetRamBank(BANK_IMAGES);
	data = gImageData+ptr;
    size = *((U16*)data);
	data += sizeof(U16);
	
/*	memset(data,0,size); */
	GetImage(data,x,y,w,h);
	/*
	if (ptr != NULL && x >= 0 && x <= LCD_WIDTH && y >= 0 && y <= LCD_HEIGHT)
	{
		
		if (ptr >= gImageData && ptr <= gImageData + 0x4000)
		{
			GetImage(ptr,x,y,w,h);
		}
	}
	*/
	SetRamBank(STD_BANK);
}

void cmd_FILLBLOCK()
{
	U16 x,y,w,h;
	U8 c;
	h = *((U16*)gStackPtr)++;
	w = *((U16*)gStackPtr)++;
	y = *((U16*)gStackPtr)++;
	x = *((U16*)gStackPtr)++;
	c = *((U8*)gStackPtr)++;
	FillBlock(c,x,y,w,h);

} 

void cmd_Delay1MS()
{
	U16 anz;
	anz = POP_STACK16;
	Delay1ms(anz);
}


void cmd_Delay1S()
{
	U16 anz;
	S16 key; 
	anz = POP_STACK16;
	DelaySecond(anz,&key);
	PUSH_STACK16(key);
}
 
void cmd_Delay250MS()
{
	U16 anz;
	S16 key;
	anz = POP_STACK16;
	Delay250ms(anz,&key);
	PUSH_STACK16(key);
}

void cmd_FILLSCREEN()
{
	U8 c;
	c = *((U8*)gStackPtr)++;
	FillScreen(c);

}

void cmd_INVERSEBLOCK()
{
	U16 x,y,w,h;
	h = *((U16*)gStackPtr)++;
	w = *((U16*)gStackPtr)++;
	y = *((U16*)gStackPtr)++;
	x = *((U16*)gStackPtr)++;
	InverseBlock(x,y,w,h);

}

void cmd_VLINE()
{
	U16 x,y,w;
	U8 c,t;
	t = *((U8*)gStackPtr)++;
	w = *((U16*)gStackPtr)++;
	y = *((U16*)gStackPtr)++;
	x = *((U16*)gStackPtr)++;
	c = *((U8*)gStackPtr)++;
	DrawVerticalLine(c,x,y,w,t);

}

void cmd_HLINE()
{
	U16 x,y,w;
	U8 c,t;
	t = *((U8*)gStackPtr)++;
	w = *((U16*)gStackPtr)++;
	y = *((U16*)gStackPtr)++;
	x = *((U16*)gStackPtr)++;
	c = *((U8*)gStackPtr)++;
	DrawHorizontalLine(c,x,y,w,t);

}

void cmd_RECTANGLE()
{
	U16 x,y,w,h;
	U8 c,t;
	t = *((U8*)gStackPtr)++;
	h = *((U16*)gStackPtr)++;
	w = *((U16*)gStackPtr)++;
	y = *((U16*)gStackPtr)++;
	x = *((U16*)gStackPtr)++;
	c = *((U8*)gStackPtr)++;
	DrawRectangle(c,x,y,w,h,t);
}

void cmd_CURSORSTATUS()
{
	SetCursorStatus(*gStackPtr++);

}

void cmd_CURSORSHAPE()
{
	U16 x,y;
	y = *((U16*)gStackPtr)++;
	x = *((U16*)gStackPtr)++;
	SetCursorShape(x,y);

}

void cmd_FONT()
{
	SetFontMode(*gStackPtr++);


}

void cmd_SCROLLUP()
{
	U16 x,y,x1,y1,o;
	U8 c,t;
	c = *((U8*)gStackPtr)++;
	o = *((U16*)gStackPtr)++;
	y1 = *((U16*)gStackPtr)++;
	x1 = *((U16*)gStackPtr)++;
	y = *((U16*)gStackPtr)++;
	x = *((U16*)gStackPtr)++;
	ScrollPageUp(x,y,x1,y1,o,c);

}

void cmd_SCROLLDN()
{
	U16 x,y,x1,y1,o;
	U8 c,t;
	c = *((U8*)gStackPtr)++;
	o = *((U16*)gStackPtr)++;
	y1 = *((U16*)gStackPtr)++;
	x1 = *((U16*)gStackPtr)++;
	y = *((U16*)gStackPtr)++;
	x = *((U16*)gStackPtr)++;
	ScrollPageDn(x,y,x1,y1,o,c);

}

void cmd_SCROLLLEFT()
{
	U16 x,y,x1,y1,o;
	U8 c,t;
	c = *((U8*)gStackPtr)++;
	o = *((U16*)gStackPtr)++;
	y1 = *((U16*)gStackPtr)++;
	x1 = *((U16*)gStackPtr)++;
	y = *((U16*)gStackPtr)++;
	x = *((U16*)gStackPtr)++;
	ScrollPageLeft(x,y,x1,y1,o,c);


}

void cmd_SCROLLRIGHT()
{
	U16 x,y,x1,y1,o;
	U8 c,t;
	c = *((U8*)gStackPtr)++;
	o = *((U16*)gStackPtr)++;
	y1 = *((U16*)gStackPtr)++;
	x1 = *((U16*)gStackPtr)++;
	y = *((U16*)gStackPtr)++;
	x = *((U16*)gStackPtr)++;
	ScrollPageRight(x,y,x1,y1,o,c);

}

void cmd_RESTORELCD()
{
	U16 x,y,w,h;
	h = *((U16*)gStackPtr)++;
	w = *((U16*)gStackPtr)++;
	y = *((U16*)gStackPtr)++;
	x = *((U16*)gStackPtr)++;
	RestoreLcd(x,y,w,h);

}

void cmd_BACKUPLCD()
{
	U16 x,y,w,h;
	h = *((U16*)gStackPtr)++;
	w = *((U16*)gStackPtr)++;
	y = *((U16*)gStackPtr)++;
	x = *((U16*)gStackPtr)++;
	BackupLcd(x,y,w,h);

}

void cmd_RESTOREHELPLCD()
{
	BackupHelpLcd();

}

void cmd_BACKUPHELPLCD()
{
	RestoreHelpLcd();

}

void cmd_SETINDION()
{
	SetIndiOn(*gStackPtr++);

}

void cmd_SETINDIOFF()
{
	SetIndiOff(*gStackPtr++);

}

void cmd_SETTONE()
{
	/* SetTone(*((U16*)gStackPtr)++); */

}

void cmd_MAKEWARNTONE()
{
	MakeWarnTone();

}

void cmd_SETPOWERMODE	()
{
	SetPowerMode(*((U16*)gStackPtr)++);

}

void cmd_MASKSYMKEY()
{

	MaskSymKey(*gStackPtr++);

}

void cmd_MASKFNTKEY()
{
	MaskFntKey(*gStackPtr++);

}

void cmd_CLEARCHARBUF()
{
	ClearKeyBuf();

}

void cmd_SETKEYTONE()
{
	SetKeyTone(*((U16*)gStackPtr)++);

}

void cmd_STRINGMENU()
{
	ICString *str;
	U16 x;
	char buf[256];
	x = POP_STACK16;
	PopString(&str);
	strcpy(buf,str->pString);
	x = StringMenu(buf,x);
	PUSH_STACK16(x);

}

void cmd_VSCROLLBAR()
{
	S16 TotalSections;
	S16 VisibleSections;
	S16 CurrentSectionIndex;
	S16 X;
	S16 Y;
	S16 Height;
	Height = *((U16*)gStackPtr)++;
	Y	   = *((U16*)gStackPtr)++;
	X      = *((U16*)gStackPtr)++;
	CurrentSectionIndex = *((U16*)gStackPtr)++;
	VisibleSections	    = *((U16*)gStackPtr)++;
	TotalSections		= *((U16*)gStackPtr)++;

	VerticalScrollBar(TotalSections, VisibleSections, CurrentSectionIndex, X, Y, Height );


}

void cmd_SHADOWEDGERECT()
{
	U16 x,y,w,h;
	h = *((U16*)gStackPtr)++;
	w = *((U16*)gStackPtr)++;
	y = *((U16*)gStackPtr)++;
	x = *((U16*)gStackPtr)++;
	ShadowEdgeRect(x,y,w,h);

}

void cmd_STRINGDIALOGBOX()
{
	ICString *str;
	U16 x;
	x = POP_STACK16;
	PopString(&str);
	x = StringDialogBox(str->pString,x);
	PUSH_STACK16(x);

}

void cmd_STRINGPHONEMENU()
{

	ICString *str1;
	U16 x;
	x = POP_STACK16;
	PopString(&str1);
	x = StringPhoneMenu(str1->pString,x);
	if (str1->Typ == TYP_STR_D) BASIC_DisposeMemory(str1->pString);
	PUSH_STACK16(x);

}

void cmd_BUTTON()
{
	S16 x,y,c;
	ICString *str1;
	c = *((U16*)gStackPtr)++;
	y = *((U16*)gStackPtr)++;
	x = *((U16*)gStackPtr)++;
	PopString(&str1);
	Button(str1->pString,x,y,c);
	if (str1->Typ == TYP_STR_D) BASIC_DisposeMemory(str1->pString);

}

void ModifyDialogResult(U16 count, U8 *d)
{
	U8 c;
	U16 i;
	for (i=0;i<count;i++)
	{
		switch(*d) 
		{
			case 0:			*d = '0';		break;
			case 1:			*d = '1';		break;
			case '0':		*d = 0;			break;
			case '1':		*d = 1;			break;
		}
		d++;
	}
	*d++ =0;

}

void cmd_CHECKBOX()
{
	U16 x,y,count;
	ICString *str1,*str2;
	char *s,c;
	
	y = POP_STACK16;
	x = POP_STACK16;
	PopString(&str2);
	PopString(&str1);
	s = str1->pString;
	count = 0;
	while ((c=*s++) != 0)
		if (c == '^') count++;
	memcpy(gPrintBuf,str2->pString,count+1);
	ModifyDialogResult(count,gPrintBuf);

	CheckBox(str1->pString,gPrintBuf,x,y);
	if (str1->Typ == TYP_STR_D) BASIC_DisposeMemory(str1->pString);
	if (str2->Typ == TYP_STR_D) BASIC_DisposeMemory(str1->pString);
	ModifyDialogResult(count,gPrintBuf);
	str1 = CreateStackString();
	str1->Len = strlen(gPrintBuf)+1;
	str1->Typ = TYP_STR_D;
	BASIC_AllocateString(str1->Len+1,&str1->pString);
	strcpy(str1->pString,gPrintBuf);
}

void cmd_RADIOBUTTON()
{
	U16 x,y,field;
	ICString *str1;
	y = POP_STACK16;
	x = POP_STACK16;
	field = POP_STACK16;

	PopString(&str1);
	memset(gPrintBuf,0,200);
	x = RadioButton(str1->pString,field,x,y);
	if (str1->Typ == TYP_STR_D) BASIC_DisposeMemory(str1->pString);
	PUSH_STACK16(x);
}


void InitVMAPI()
{
	SetCmdAPI(CMD_API_FILLBLOCK,cmd_FILLBLOCK);
	SetCmdAPI(CMD_API_FILLSCREEN,cmd_FILLSCREEN);
	SetCmdAPI(CMD_API_INVERSEBLOCK,cmd_INVERSEBLOCK);
	SetCmdAPI(CMD_API_VLINE,cmd_VLINE);
	SetCmdAPI(CMD_API_HLINE,cmd_HLINE);
	SetCmdAPI(CMD_API_RECTANGLE,cmd_RECTANGLE);
	SetCmdAPI(CMD_API_CURSORSTATUS,cmd_CURSORSTATUS);
	SetCmdAPI(CMD_API_CURSORSHAPE,cmd_CURSORSHAPE);
	SetCmdAPI(CMD_API_FONT,cmd_FONT);
	SetCmdAPI(CMD_API_SCROLLUP,cmd_SCROLLUP);
	SetCmdAPI(CMD_API_SCROLLDN,cmd_SCROLLDN);
	SetCmdAPI(CMD_API_SCROLLLEFT,cmd_SCROLLLEFT);
	SetCmdAPI(CMD_API_SCROLLRIGHT,cmd_SCROLLRIGHT);
	SetCmdAPI(CMD_API_RESTORELCD,cmd_RESTORELCD);
	SetCmdAPI(CMD_API_BACKUPLCD,cmd_BACKUPLCD);
	SetCmdAPI(CMD_API_RESTOREHELPLCD,cmd_RESTOREHELPLCD);
	SetCmdAPI(CMD_API_BACKUPHELPLCD,cmd_BACKUPHELPLCD);
	SetCmdAPI(CMD_API_SETINDION,cmd_SETINDION);
	SetCmdAPI(CMD_API_SETINDIOFF,cmd_SETINDIOFF);
	SetCmdAPI(CMD_API_SETTONE,cmd_SETTONE);
	SetCmdAPI(CMD_API_MAKEWARNTONE,cmd_MAKEWARNTONE);
	SetCmdAPI(CMD_API_SETPOWERMODE	,cmd_SETPOWERMODE	);
	SetCmdAPI(CMD_API_MASKSYMKEY,cmd_MASKSYMKEY);
	SetCmdAPI(CMD_API_MASKFNTKEY,cmd_MASKFNTKEY);
	SetCmdAPI(CMD_API_CLEARCHARBUF,cmd_CLEARCHARBUF);
	SetCmdAPI(CMD_API_SETKEYTONE,cmd_SETKEYTONE);
	SetCmdAPI(CMD_API_STRINGMENU,cmd_STRINGMENU);
	SetCmdAPI(CMD_API_VSCROLLBAR,cmd_VSCROLLBAR);
	SetCmdAPI(CMD_API_SHADOWEDGERECT,cmd_SHADOWEDGERECT);
	SetCmdAPI(CMD_API_STRINGDIALOGBOX,cmd_STRINGDIALOGBOX);
	SetCmdAPI(CMD_API_STRINGPHONEMENU,cmd_STRINGPHONEMENU);
	SetCmdAPI(CMD_API_BUTTON,cmd_BUTTON);
	SetCmdAPI(CMD_API_CHECKBOX,cmd_CHECKBOX);
	SetCmdAPI(CMD_API_RADIOBUTTON,cmd_RADIOBUTTON);
	SetCmdAPI(CMD_API_LOADIMAGE,cmd_API_LoadImage);
	SetCmdAPI(CMD_API_ALLOCIMAGE,cmd_API_AllocImage);
	SetCmdAPI(CMD_API_PUTIMAGE,cmd_API_PutImage);
	SetCmdAPI(CMD_API_GETIMAGE,cmd_API_GetImage);
	SetCmdAPI(CMD_API_DELAY1MS,cmd_Delay1MS);
	SetCmdAPI(CMD_API_DELAY1S,cmd_Delay1S);
	SetCmdAPI(CMD_API_DELAY250MS,cmd_Delay250MS);
}

void cmd_APICall()
{
   U8 opcode;
   VMCOMMAND *cmd;
   opcode = *gCodePtr++;
   if (opcode > CMD_API_MAX) ERROR;
   cmd = gAPICmdJumpTable[opcode];
   cmd();
}

