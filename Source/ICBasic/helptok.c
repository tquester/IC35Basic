
#include "ic35sdk.h"
#include "data.h"
#include "icstdioq.h"
#include "vm.h"

void HelpToken()
{
	BasicToken *tok;
	U8 line = 0;
	U8 id;
	U8 found;
	U8 *s;

	U8 *fn;
	ic_cls(&gTerm);
	strcpy(gTempBuf,"Listing statements\n");
	ic_puts(&gTerm,gTempBuf);

	tok = gBasToken;
	while (tok->token != 0)
	{
		fn = "";
		id = 0;
		found = FALSE;
		strcpy(gTempBuf,fn);
		strcat(gTempBuf," ");
		strcat(gTempBuf,tok->name);
		strcat(gTempBuf," ");
		if (tok->syntax != NULL)
			strcat(gTempBuf,tok->syntax);
		found = TRUE;
		ic_puts(&gTerm,gTempBuf);
		gTerm.curX = 100;
		s = NULL;
		switch(tok->returntype)
		{
			case TYP_8:		s = "byte"; break;
			case TYP_16:	s = "integer"; break;
			case TYP_32:	s = "long"; break;
			case TYP_F:		s = "float"; break;
			case TYP_STR_C:	s = "string"; break;
		}
		if (s != NULL)
		{
			strcpy(gTempBuf,s);
			strcat(gTempBuf,"=");
			strcat(gTempBuf,tok->name);
			if (tok->help != NULL)
			{
				strcat(gTempBuf,"(");
				strcat(gTempBuf,tok->help);
				strcat(gTempBuf,")");
			}
			ic_puts(&gTerm,gTempBuf);
		}
		else
		{
			if (tok->help != NULL)
			{
				strcpy(gTempBuf,tok->help);
				ic_puts(&gTerm,gTempBuf);
			}
		}
 		strcpy(gTempBuf,"\n");
		ic_puts(&gTerm,gTempBuf);
		if (++line > 10) {
			GetKey();
			ic_cls(&gTerm);
			line=0;
		}
		tok++;

	}
	GetKey();
}