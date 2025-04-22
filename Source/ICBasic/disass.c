
#include "ic35sdk.h"
#include "data.h"
#include "vm.h"
#include "icstdioq.h"

void DisassOneParam_8(char *cmd);
void DisassTyp(U8 typ);
void DisassNoParam(char *cmd);
void GetAPIOpName(U8 *target, U8 apiOpCode);
void CommandNameFromOpCode(U8 *target, U8 opcode);

void	 DisassLine()
{
	U8 *s,*d,c;
	
	S16 line;
	gTempBuf[7] = ';';
	line = *((U16*)gCodePtr)++;
	itoa(line,gPrintBuf,10);
	s = gPrintBuf;
	d = gTempBuf+8;
	while ((c = *s++) != 0)
	{
		*d++=c;
	}
	*d++=' ';
	d = gTempBuf+14;


	s = gSource;
	while ((c=*s) != 0) 
	{
		if (line <= 0) {
			while ((c = *s++) != 0) {
				if (c == 10 || c == 13) break;
				if (d-gTempBuf > 69) break;
				*d++=c;
			}
			break;
		}
		if (c == 10) line--;
		s++;
	}
	*d = 0;
}
void CopyCmd(char *cmd)
{
	memcpy(gTempBuf+20,cmd,strlen(cmd));
}

void CopyI16(char *ext)
{
	Variable *r;
	char buf[15];
	U16 x1;
	U16 i;
	x1 = *((U16*)gCodePtr)++;
	buf[0] = '[';
	buf[1] = 0;
	strcat(buf,ext);
	
	itoa(x1,buf+strlen(buf),10);
	strcat(buf,"]");
	strcpy(gTempBuf+30,buf);
	gTempBuf[strlen(gTempBuf)] = ' ';
	r = gVariables;
	for (i=0;i<gLastVar;i++)
	{
		if (r->m_Offset == x1) {
			gTempBuf[39] = ';';
			strcpy(gTempBuf+40,r->m_Name);
			break;
		}
		r++;
	}

}

void Copy16()
{
	U16 x1;
	x1 = *((U16*)gCodePtr)++;
	itoa(x1,gTempBuf+40,10);
}

void Copy8()
{
	U16 x1;
	x1 = *gCodePtr++;
	itoa(x1,gTempBuf+40,10);
}

void DisassAPI()
{
	U8 api;
	CopyCmd("API");

	api = *gCodePtr++;
	GetAPIOpName(gTempBuf+25, api);
}

void DisassTyp(U8 typ)
{
	char *p;
	U8 o;
	o = '_';
	switch(typ)
	{
		case TYP_8:   o = 'b';	break;
		case TYP_16:  o = 'i';  break;
		case TYP_32:  o = 'l';  break;
		case TYP_F:	  o = 'f';  break;
	}	
	p = gTempBuf+26;
	while (*p == ' ')
		p--;
	p++;
	p[0] = '.';
	p[1] = o;
	//gTempBuf[pos] = o;
}

void DisassLoadStringC()
{
	U16 len;
	len = *((U16*)gCodePtr)++;
	CopyCmd("LDSTR_C");
	gTempBuf[26] = '"';
	strcpy(gTempBuf+27,gCodePtr);
	gCodePtr +=len;
	strcat(gTempBuf,"\"");
	

}
void DisassLS(U8 opcode, char *cmd)
{
	U8 x,typ,adr;
	U16 x1;
	U32 x2;
	float x3;

	
	switch(opcode)
	{
		case CMD_PUSH_BP:		CopyCmd("PUSH BP");			break;	
		case CMD_POP_BP:		CopyCmd("POP BP");			break;	
		case CMD_PUSH_VP:		CopyCmd("PUSH VP");			break;	
		case CMD_POP_VP:		CopyCmd("POP VP");			break;	
		case CMD_PUSH_F:		CopyCmd("PUSH F");			break;	
		case CMD_POP_F:			CopyCmd("POP F");			break;	
		case CMD_LD_BP_SP:		CopyCmd("LD BP,SP");		break;	
		case CMD_LD_SP_BP:		CopyCmd("LD SP,BP");		break;	
		case CMD_ADDSP:			DisassOneParam_8("ADDSP");  break;
		
		default:
			if ((opcode & 0xFC) == CMD_DUP)
			{
				CopyCmd("DUP");
				DisassTyp((U8)(opcode & 0x3));
			}
			else
			{
				CopyCmd(cmd);
				DisassTyp((U8)(opcode & 0x3));
				if (opcode & ADR_INDEX) gTempBuf[27] = 'I';

				adr = (opcode & 0x0C);
				switch(adr)
				{
					case ADR_C:
							gTempBuf[28] = '#';
							typ = opcode & 0x3;
							if (typ ==  TYP_8) {
								x = *gCodePtr++;
								itoa(x,gTempBuf+30,10);
							}
							if (typ == TYP_16) {
								x1 = *((U16*)gCodePtr)++;
								itoa(x1,gTempBuf+30,10);
							}
							if (typ == TYP_32) {
								x2 = *((U32*)gCodePtr)++;
								ltoa(&x2,gTempBuf+30,10);
							}
							if (typ == TYP_F) {
								x3 = *((float*)gCodePtr)++;
								ftoa(x3,gTempBuf+30);
							}
							break;
					case ADR_V:
	//						gTempBuf[25] = 'V';
							CopyI16("");
							
							break;

					case ADR_BP:
	//						gTempBuf[25] = 'B';
	//						gTempBuf[26] = 'P';
							CopyI16("BP+");
							break;
				}
			}
	}
}

void DisassArith(U8 opcode)
{
	U8 op;
	op =  opcode & 0xFC;
	switch(op)
	{
		case CMD_NOT:		CopyCmd("NOT");			break;
		case CMD_AND:		CopyCmd("AND");			break;
		case CMD_OR:		CopyCmd("OR");			break;
		case CMD_CMP:		CopyCmd("CMP");			break;
		case CMD_ADD:		CopyCmd("ADD");			break;
		case CMD_SUB:		CopyCmd("SUB");			break;
		case CMD_MULT:		CopyCmd("MULT");		break;
		case CMD_DIV:		CopyCmd("DIV");			break;
	}
	DisassTyp((U8)(opcode & 0x3));
}

void DisassNoParam(char *cmd)
{
	CopyCmd(cmd);
}

void DisassOneParam_8(char* cmd)
{
	CopyCmd(cmd);
	Copy8();

}

void DisassOneParam_16(char* cmd)
{
	CopyCmd(cmd);
	Copy16();

}

void DisassExt(U8 opcode)
{
	U8 typ1;
	U8 typ2;

	typ1 = opcode & 0x3;
	opcode >>= 2;
	typ2 = opcode & 0x3;

	CopyCmd("EXT");
	DisassTyp(typ2);
	DisassTyp(typ1);
}

void DisassJump(U8 opcode)
{
	U8 *p,cond;
	if (opcode == CMD_CALL)
		CopyCmd("CALL");
	else
		CopyCmd("JUMP");
	p = gTempBuf+25;
	
	cond = opcode & 7;
	switch(cond) {
		case COND_NULL			:							break;
		case COND_Z				: *p = 'Z';					break;
		case COND_NZ			: *p++ = 'N'; *p = 'Z';		break;
		case COND_GT			: *p++ = 'G'; *p = 'T';		break;
		case COND_GE			: *p++ = 'G'; *p = 'E';		break;
		case COND_LT			: *p++ = 'L'; *p = 'T';		break;
		case COND_LE			: *p++ = 'L'; *p = 'E';		break;
	}

	Copy16();
}

void DisassIncDec(U8 *cmd)
{
	U8 c,typ,adr;
	CopyCmd(cmd);
	
	c = *gCodePtr++;
	typ = c & 3; 
	adr = c & 0xff-3; 
	DisassTyp(typ);
	switch(adr)
	{
		case ADR_V:
			 CopyI16("");
			 break;
	}
}
			 
	

void DisassBi(U8 opcode, char *buf)
{
	U8 typ;

	typ = opcode & 3;
	CopyCmd(buf);
	DisassTyp(typ);
}

U8 CreateDisassLine()
{
	U8 opcode;
    VMCOMMAND *cmd;
	U8 i,c,*s,*d,*p,*cp,*start;
	U8 subcode;
		memset(gTempBuf,32,75);
		gTempBuf[75] = 0;
		itoa((U16)(gCodePtr-gCode),gTokenBuf,10);
		d = gTempBuf;
		s = gTokenBuf;
		for (i=strlen(gTokenBuf);i<=4;i++)
			*d++='0';
		while ((c = *s++) != 0)
			*d++=c;
/*		memcpy(gTempBuf,gTokenBuf,strlen(gTokenBuf)); */
		opcode = *gCodePtr++;
	    cmd = gCmdJumpTable[opcode];

/*		
		if (opcode == CMD_HALT) {
			CopyCmd("HALT");
			return 1;
		}


*/
		start = gCodePtr-1;
		switch(opcode)
		{

/*
				case CMD_RAND_F:		DisassNoParam("RAND_F");		break;
				case CMD_RANDOMIZE:		DisassNoParam("RANDOMIZE");		break;
				case CMD_SIN:			DisassNoParam("SIN");			break;
				case CMD_COS:			DisassNoParam("COS");			break;
				case CMD_TAN:			DisassNoParam("TAN");			break;
				case CMD_TIME_S:		DisassNoParam("TIMES");			break;
				case CMD_DATE_S:		DisassNoParam("DATES");			break;
				case CMD_ASC:			DisassNoParam("ASC");			break;
				case CMD_CHR:			DisassNoParam("CHR");			break;
				case CMD_VAL:			DisassNoParam("VAL");			break;
				case CMD_STR:			DisassNoParam("STR");			break;
				case CMD_LEFT:			DisassNoParam("LEFT");			break;
				case CMD_RIGHT:			DisassNoParam("RIGHT");			break;
				case CMD_PLOT:			DisassNoParam("PLOT");			break;
				case CMD_LINE:			DisassNoParam("LINE");			break;
				case CMD_CLS:			DisassNoParam("CLS");			break;
				case CMD_GETKEY:		DisassNoParam("GETKEY");		break;
*/
				case CMD_ABS_8:			DisassNoParam("ABS.b");			break;
				case CMD_ABS_16:		DisassNoParam("ABS.i");			break;
				case CMD_ABS_32:		DisassNoParam("ABS.l");			break;
				case CMD_ABS_F:			DisassNoParam("ABS.f");			break;
				case CMD_MID:			DisassNoParam("MID");			break;
			    case CMD_API:           DisassAPI();					break;
				case CMD_INC:			DisassIncDec("INC");			break;
				case CMD_DEC:			DisassIncDec("DEC");			break;
				case CMD_LOADF_Z:		DisassNoParam("LOADF_Z");		break;
				case CMD_LOADF_NZ:		DisassNoParam("LOADF_NZ");		break;
				case CMD_LOADF_LE:		DisassNoParam("LOADF_LE");		break;
				case CMD_LOADF_LT:		DisassNoParam("LOADF_LT");		break;
				case CMD_LOADF_GE:		DisassNoParam("LOADF_GE");		break;
				case CMD_LOADF_GT:		DisassNoParam("LOADF_GT");		break;
				case CMD_LOAD_STR_V:	DisassOneParam_16("LOADSTRV");	break;
				case CMD_STORE_STR_V:	DisassOneParam_16("STORESTRV");	break;
				case CMD_LOAD_STR_VI:	DisassOneParam_16("LOADSTRVI");	break;
				case CMD_STORE_STR_VI:	DisassOneParam_16("STORESTRVI");break;
				case CMD_STRINGADD:		DisassNoParam("ADDSTR");		break;
				case CMD_STRINGCMP:		DisassNoParam("CMPSTR");		break;
				case CMD_LOAD_STR_C:	DisassLoadStringC();			break;
				case CMD_PRINT_STR:     DisassNoParam("PRINT STR");		break;
				case CMD_SOURCELINE:	DisassLine();					break;
				case CMD_ADDSP:			DisassOneParam_8("ADDSP");		break;
				case CMD_INPUTI +TYP_8:		DisassNoParam("INPUT8");		break;
				case CMD_INPUTI +TYP_16:		DisassNoParam("INPUT16");		break;
				case CMD_INPUTI +TYP_32:		DisassNoParam("INPUT32");		break;
				case CMD_INPUTI +TYP_F:		DisassNoParam("INPUTF");		break;
				case CMD_INPUT_STR:		DisassNoParam("INPUT_STR");		break;
				case CMD_PRINT_TAB:		DisassNoParam("PRINT_TAB");		break;
				case CMD_PRINT_NL:		DisassNoParam("PRINT_NL");		break;
				case CMD_HALT:			DisassNoParam("HALT");			break;
				case CMD_RTS:			DisassNoParam("RTS");			break;
				default:  
					subcode = opcode >> 5;
					switch(subcode)
					{
						case 0:			DisassLS(opcode,"LOAD");		break;
						case 1:			DisassLS(opcode,"STORE");		break;
						case 2:			DisassArith(opcode);			break;
						case 3:			DisassJump(opcode);				break;
						default:										
							if ((opcode & 0xf0) == 0xf0)  DisassExt(opcode);
							else if ((U8)(opcode & CMD_PRINTI) == CMD_PRINTI) { DisassNoParam("PRINT"); DisassTyp(opcode & 3); }
							else {
								CommandNameFromOpCode(gPrintBuf,opcode);
								DisassNoParam(gPrintBuf);
							}
							break;


					}
					break;

		}


		if (opcode != CMD_SOURCELINE)
		{
			p = gTempBuf+7;
			for (i=0,cp = start; cp < gCodePtr; cp++)
			{
				if (i > 3) break;
				HexByte(&p,*cp);		
				p++;
				i++;
			}
		}




		strcpy(gTokenBuf,"\r\n");

		strcat(gTempBuf,gTokenBuf);
		return 0;

}

void Disass(U8 *adr, U8 *end, U8 printit)
{
	U8 *endptr;
	U8 line;
	U16 key,r;
	FILE_IDEN iden;

	U8   **pages;
	U8   maxPage, curPage;

	maxPage = 0;
	curPage = 0;

	endptr = end;
	pages = (U8**)gStack;

	InitICTerm(&gTerm,gPrintBuf,sizeof(gPrintBuf)-1);


	line = 0;
	strcpy(gTempBuf,"MMCard1\\BASIC.DIS");

	r = mOpenFile(gTempBuf,CREATE_ALWAYS, &iden);
	if (r != MMC_ERROR_SUCCESS) {
		ic_puts(&gTerm,"Error opening the disass file");
		return;
	}

	gCodePtr = adr;
	

	pages[0] = gCodePtr;
	maxPage = 1;
	while (TRUE)
	{
		if (gCodePtr > endptr) break;

		if (CreateDisassLine() == 1)
			break;
	
		if (printit)
		{
			ic_puts(&gTerm,gTempBuf);
			if (gTerm.curY+gTerm.lineHeight+12 > gTerm.y2)
			{
				strcpy(gTempBuf,"up=prev page, esc = exit, any key = next page");
				FillBlock(1,0,LCD_HEIGHT-16,LCD_WIDTH-1,15);
				WriteText(0,gTempBuf,0,LCD_HEIGHT-16);
				key = GetKey();
				if (key == ESC) break;
				pages[++curPage] = gCodePtr;
				if (key == UP && curPage > 0) {
					if (curPage >= 2) {
						curPage-=2;
						gCodePtr = pages[curPage];
					}
					
				} 
				ic_cls(&gTerm);
			}


			/*
			line++;
			if (line == 12)
			{
				key = GetKey();
				cls();
				line = 0;
			}
			*/
		}


		mWriteToFile(&iden,gTempBuf,(U16)strlen(gTempBuf));
		if (r != MMC_ERROR_SUCCESS) {
			ic_puts(&gTerm,"Error writing the disass file");
			mCloseFile(&iden);
			return;
		}
/*		break; */

	}
	mCloseFile(&iden);
}

U8 DisassSingleLine()
{
	S16 key,y,i;
	U8 *ptr = gCodePtr;
	U8 *p;
	CreateDisassLine();
	gCodePtr = ptr;
	FillBlock(1,0,LCD_HEIGHT-14,239,14);
	FillBlock(1,LCD_WIDTH-40,0,LCD_WIDTH-40,LCD_HEIGHT-14);
	WriteText(0,gTempBuf,0,LCD_HEIGHT-14);
	y = 0;
	for (p = gStackPtr;p<=gStack+gSettings.Size_Stack;p++)
	{   
		i = *p;
		itoa(i,gTempBuf,10);
		WriteText(0,gTempBuf,LCD_WIDTH-40,y);
		y+=12;
		if (y > LCD_HEIGHT-14) break;
	}
	key = GetKey();
	if (key == ESC) return 1;
	return 0;
}	

U8 DebugShowLine(U16 line)
{
	U8 *d,*s,c;
	S16 key;
	s = gSource;
	strcpy(gPrintBuf,"Line ");
	itoa(line,gTempBuf,10);
	strcat(gPrintBuf,gTempBuf);
	strcat(gPrintBuf," ");
	FillBlock(1,0,0,239,12);

	d = gPrintBuf+strlen(gPrintBuf);

	while ((c=*s) != 0) 
	{
		if (line <= 0) {
			while ((c = *s++) != 0) {
				if (c == 10 || c == 13) break;
				if (d-gPrintBuf > 69) break;
				*d++=c;
			}
			break;
		}
		if (c == 10) line--;
		s++;
	}
	*d++=0;
	WriteText(0,gPrintBuf,0,0);
	key = GetKey();
	if (key == ESC) return 1;
	return 0;

}


