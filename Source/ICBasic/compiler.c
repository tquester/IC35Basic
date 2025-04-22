/*
------------------------------------------------
Grammar
------------------------------------------------
var     	::= letter ( letter | digit)*
name    	::= var
number  	::= digit ( digit )*
cstring  	::= '"' (U8)* '"'
size    	::= number
label   	::= var
typ     	::= 'BYTE' | 'SHORT' | 'INTEGER' | 'LONG' | 'STRING'
function	::= name ( ('(' expr|sexpr (',' expr|sexpr)*)
group   	::= '(' expr ')'
expr1   	::= integer | group | var | function
expr2   	::= expr1 (('*' expr1) | ('/' expr1))*
expr3   	::= expr2 (('+' expr2) | ('-' expr2))*
expr4   	::= expr3 (( '>' expr3) | ( '<' expr3) | ( '>=' expr3) | ( '<=' expr3) | ( '=' expr3) 
expr    	::= ('NOT' expr4) | ( 'AND' expr4) | ( 'OR' expr4)) | expr4 
sgroup  	::= '(' sexpr ')'
sexpr1  	::= cstring | sgroup | var | function
sexpr   	::= sexpr1 ('+' sexpr1)*

ifstmt1 	::= 'IF' expr 'THEN'
ifstmt  	::= ifstmt1 NEWLINE | stmts ( 'ELSE' stmts );

stmts   	::= stmt (':' stmt)*
stmt    	::= var '=' expr				; Assignment
stmt    	::= var ':'					; Label
stmt    	::= 'DOT' expr,expr,expr		
stmt    	::= 'PRINT' expr|sexpr ( (';' expr|sexpr) | (',' expr|sexpr) )*
stmt    	::= 'INPUT' expr|sexpr ( (';' expr|sexpr) | (',' expr|sexpr) )* var ((',' var) | (';' var))*
stmt    	::= 'FOR' var 'to' expr		
stmt    	::= 'IF'  expr 'THEN' 
stmt    	::= 'NEXT'   (var)			
stmt    	::= 'GOTO'   label
stmt    	::= 'GOSUB'  label
stmt    	::= 'RETURN'
stmt    	::= ifstmt 
stmt    	::= 'ELSE' (stmts)
stmt    	::= 'END' 'IF'
stmt    	::= 'DIM' var ('(' dim (',' number)*) 'AS' typ ('[' size ']')

-------------------------------------------------
*/

#include "ic35sdk.h"
#include "data.h"
#include "compiler.h"
#include "vm.h"
#include "icmemory.h"
#include "ui.h"
#include "icstdioq.h"
#include "compapi.h"

U8 Expr(U8 ResultTye);
U8 Expr0();

U8 _isdigit(U8 c)
{
	return c >= '0' && c <= '9';
}

double _atof(char *s)
{
	char c;
	double faktor = 10;
	double n = 0;
	double d = 0;
	while ((c = *s++) != 0)
	{
		if (c == '.') break;
		if (!_isdigit(c)) break;
		n = 10 * n + c - '0';
	}
	if (c == '.') 
	{
		while ((c = *s++) != 0)
		{
			if (!_isdigit(c)) break;
			d += (c-'0') / faktor;
			faktor *=10;
		}
		n += d;
	}
	return n;
}

void Disass(U8 *adr, U8 *endptr, U8 printit);
U8 DisassSingleLine();

U8 CompileStatement();
void CompileStatements();

U8 IsStringTyp(U8 typ);

void UngetToken();


void InitToken(BasicToken **t, U8 *s, U8 id)
{
	(*t)->name = s;
	(*t)->token = id;
	(*t)++;
}




void InitSyntax(CompileToken **pSyn,U8 token, U8 op1, U8 op2, U8 rettype, U8 *syntax)
{
	CompileToken *syn	= *pSyn;
	syn->token			= token;
	syn->op1			= op1;
	syn->op2			= op2;
	syn->Syntax			= syntax;
	syn->returntype		= rettype;
	syn++;
	*pSyn = syn;
}

void InsertDebugLine()
{
	if (gCompilerInsertDebug)
	{
		*gCodePtr		++= CMD_SOURCELINE;
		*((U16*)gCodePtr) ++= gLine-1;
	}
}

/* #define PUSH_TYPE(x) *(--gStackPtr) = x */
#define POP_TYPE     *gStackPtr++

void PUSH_TYPE(U8 typ)
{
	if (gStackPtr-gStack > gSettings.Size_Stack)
		gErrorCode = ERR_OVERFLOW_ON_EXPR;
	*(--gStackPtr) = typ;
	if (gStackPtr-gStack > gSettings.Size_Stack)
		gErrorCode = ERR_OVERFLOW_ON_EXPR;
}

U8 PeekTypIsString() 
{
	return IsStringTyp(*gStackPtr);
}
							
void InitParser()
{
	U8 **s;
/*
	BasicToken     *tok;
	CompileToken   *syn;

	tok = gBasToken;
	syn = gCompileToken;


	InitToken(&tok,"PRINT",OP_PRINT);
	InitToken(&tok,"INPUT",OP_INPUT);
	InitToken(&tok,"DIM" ,OP_DIM);
	InitToken(&tok,"AS" ,OP_AS);
	InitToken(&tok,"IF" ,OP_IF);
	InitToken(&tok,"THEN" ,OP_THEN);
	InitToken(&tok,"ELSE" ,OP_ELSE);
	InitToken(&tok,"END" ,OP_END);
	InitToken(&tok,"INTEGER" ,OP_INTEGER);
	InitToken(&tok,"INT" ,OP_INTEGER);
	InitToken(&tok,"LONG" ,OP_LONG);
	InitToken(&tok,"SHORT" ,OP_SHORT);
	InitToken(&tok,"BYTE" ,OP_SHORT);
	InitToken(&tok,"STRING" ,OP_STRING);
	InitToken(&tok,"FLOAT" ,OP_FLOAT);
	InitToken(&tok,"FOR" ,OP_FOR);
	InitToken(&tok,"TO" ,OP_TO);
	InitToken(&tok,"NEXT" ,OP_NEXT);
	InitToken(&tok,"GOTO" ,OP_GOTO);
	InitToken(&tok,"GOSUB" ,OP_GOSUB);
	InitToken(&tok,"RETURN", OP_RETURN);
	InitToken(&tok,"DOT", OP_DOT);
	InitToken(&tok,"LINE",OP_LINE);
	InitToken(&tok,"CLS",OP_CLS);
	InitToken(&tok,"REM",OP_REM);
	InitToken(&tok,"NOT",OP_NOT);
	InitToken(&tok,"AND",OP_AND);
	InitToken(&tok,"OR ",OP_OR);
	InitToken(&tok,"GOTOXY",OP_GOTOXY);
	InitToken(&tok,"GETKEY",OP_GETKEY);
	InitToken(&tok,"MID",OP_MID);
	InitToken(&tok,"LEFT",OP_LEFT);
	InitToken(&tok,"RIGHT",OP_RIGHT);
	InitToken(&tok,"LEN",OP_LEN);
	InitToken(&tok,"ASC",OP_ASC);
	InitToken(&tok,"CHR",OP_CHR);
	InitToken(&tok,"STR",OP_STR);
	InitToken(&tok,"VAL",OP_VAL);
	InitToken(&tok,"STR",OP_STR);
	InitToken(&tok,"RANDOMIZE",OP_RANDOMIZE);
	InitToken(&tok,"RAND",OP_RAND);
	InitToken(&tok,"RND",OP_RAND);
	InitToken(&tok,"SIN",OP_SIN);
	InitToken(&tok,"COS",OP_COS);
	InitToken(&tok,"TAN",OP_TAN);
	InitToken(&tok,"DATE",OP_DATE);
	InitToken(&tok,"TIME",OP_TIME);


	InitSyntax(&syn,OP_LINE,	CMD_LINE,0,			TYP_NULL, "biiii");
	InitSyntax(&syn,OP_GOTOXY,  CMD_GOTOXY,0,		TYP_NULL,"bb");
	InitSyntax(&syn,OP_DOT,		CMD_PLOT,0,			TYP_NULL,"bbb");
	InitSyntax(&syn,OP_RANDOMIZE,CMD_RANDOMIZE,0,	TYP_NULL,"");

	InitAPITokens(&tok,&syn);

	InitSyntax(&syn,OP_LEN,		CMD_LEN,0,			TYP_16, "s");
	InitSyntax(&syn,OP_ASC,		CMD_ASC,0,			TYP_16, "s");
	InitSyntax(&syn,OP_CHR,		CMD_CHR,0,			TYP_STR_C,"i");
	InitSyntax(&syn,OP_VAL,		CMD_VAL,0,			TYP_F,"s");
	InitSyntax(&syn,OP_STR,		CMD_STR,0,			TYP_STR_C,"f");
	InitSyntax(&syn,OP_MID,		CMD_MID,0,			TYP_STR_C,"sii");
	InitSyntax(&syn,OP_LEFT,	CMD_LEFT,0,			TYP_STR_C,"si");
	InitSyntax(&syn,OP_RIGHT,	CMD_RIGHT,0,		TYP_STR_C,"si");
	InitSyntax(&syn,OP_RAND,	CMD_RAND_F,0,			TYP_F,"");
	InitSyntax(&syn,OP_SIN,     CMD_SIN,0,          TYP_F,"f");
	InitSyntax(&syn,OP_COS,     CMD_SIN,0,          TYP_F,"f");
	InitSyntax(&syn,OP_TAN,     CMD_SIN,0,          TYP_F,"f");
	InitSyntax(&syn,OP_DATE,	CMD_DATE_S,0,		TYP_STR_C,"");
	InitSyntax(&syn,OP_TIME,	CMD_TIME_S,0,		TYP_STR_C,"");
	
	InitSyntax(&syn,0,0,0,TYP_NULL, NULL);
	InitToken(&tok,NULL,0);
	syn = gCompileToken;
	while (syn->token != 0)
		syn++;
	gFuncSyntax = syn+1;
*/
	__nl = "\n";
	__errUnexp = "Unexpeced error";

	s = Errors;
	Errors[ERR_OK				     ] = "OK";
	Errors[ERR_EXPECED_VAR			 ] = "Expected variable";
	Errors[ERR_EXPECED_AS			 ] = "Expected as";
	Errors[ERR_EXPECED_TYPE		     ] = "Expected type";
	Errors[ERR_VAR_NOT_DEF			 ] = "Variable not defined";
	Errors[ERR_EXPECED_EQUAL		 ] = "Expected =";
	Errors[ERR_EXPECED_NEWLINE		 ] = "Expteted newline";
	Errors[ERR_EXPECED_TO			 ] = "Expected To";
	Errors[ERR_VAR_NOT_MATCH		 ] = "Variables do not match";
	Errors[ERR_SYNTAX				 ] = "Syntax Error";
	Errors[ERR_END                   ] = "End";
	Errors[ERR_NOT_A_VAR			 ] = "Not a variable";
	Errors[ERR_OP_NOT_A_LABEL        ] = "Not a label";
	Errors[ERR_LABEL_NOT_DEFINED     ] = "Label not defined";
	Errors[ERR_NEWVAR_IN_PASS2		 ] = "New variable in pass2 (compiler error?)";
	Errors[ERR_EXPECTED_BRACKET		 ] = "Expected (";
	Errors[ERR_OVERFLOW_ON_EXPR      ] = "Stack overflow on expression evaluation";
	Errors[ERR_OUT_OF_MEMORY         ] = "Out of memory";
	Errors[ERR_FUNCTION_AS_STATEMENT ] = "Functions can not be called";
	__errUnexp = "Unexpected Error";
	__nl       = "\n";

}


/*  ------------------------------------------------------------------
    Scanner 
	-------------------------------------------------------------------
*/

void _print(U8 *s)
{
	strcpy(gPrintBuf,s);
	ic_puts(&gTerm,gPrintBuf);
}

void _printi(U16 i)
{
	itoa(i,gPrintBuf,10);
	ic_puts(&gTerm,gPrintBuf);
}

void _printl(U32 i)
{
	ltoa(&i,gPrintBuf,10);
	strcat(gPrintBuf," ");
	ic_puts(&gTerm,gPrintBuf);
}


void MsgI(S16 i)
{
	itoa(i,gPrintBuf,10);
	ic_puts(&gTerm,gPrintBuf);
	strcpy(gPrintBuf," ");
	ic_puts(&gTerm,gPrintBuf);
}

void ShowLine(U8 *s)
{
	U8 *d = gStringBuf;
	U8 c;
	while ((c = *s++) != 0)
	{
		if (c == 10 || c == 13) break;
		*d++ = c;
	}
	*d++ = '\n';
	*d++ = 0;
	if (strcmp(gStringBuf,"\n") != 0)
		ic_puts(&gTerm,gStringBuf);


}

U32 _atol(U8 *s)
{
	long r = 0;
	U8 c;
	while ((c = *s++) != 0)
	{
		if (c < '0' || c > '9') break;
		c -= '0';
		r *= 10;
		r += c; 
	}
	return r;
}

U8 _isspace(U8 c)
{
	return (c == 32 || c == 9 || c == 7);
}

/* test if the U8 is an operator or an string */
U8 _isop(U8 c)
{
	U8 r = 0;
	U8 *d;
	
	switch(c)
	{
		case ',':   r = OP_KOMMA;   break;
		case ';':   r = OP_SEMIKOLON; break;
		case '+':	r = OP_ADD;		break;
		case '-':	r = OP_SUB;		break;
		case '*':	r = OP_MULT;	break;
		case '/':	r = OP_DIV;		break;
		case '=':	r = OP_EQUAL;	break;
		case ':':	r = OP_COLON;	break;
		case '(':	r = OP_BOPEN;	break;
		case ')':	r = OP_BCLOSE;	break;
		case '^':	r = OP_POWER;	break;
		case '?':	r = OP_PRINT;	break;
		case 187:
		case 155:
		case '>':	if (*gSourcePtr == '=') 
						{
						   gSourcePtr++;
						   r = OP_BIGGEREQUAL;
						}
					   else
						   r = OP_BIGGER;
					break;
				
		case 139:
		case 171:
		case '<':	if (*gSourcePtr == '=') 
						{
						   gSourcePtr++;
						   r = OP_SMALLEREQUAL;
						}
					else if (*gSourcePtr == '>')

						{
						   *gSourcePtr++;
						   r = OP_UNEQUAL;
						}
						else
							r = OP_SMALLER;
					break;
		case '"':	r = OP_STRING;
			        d = gTempBuf;
					while ((c = *gSourcePtr++) != 0)
					{
						/* if (d >= &gTempEnd-2) break; */
						if (c == '"') break;
						*d++=c;
					}
					*d++= 0;
					
	}
	return r;
}

U8 _isnum(c)
{
	U8		r = 0;
	U8		hasPoint = FALSE;
	U8	   *d = gTempBuf;
	
	while (c >= '0' && c <= '9' || c == '.') {
		if (c == '.') 
			 hasPoint = TRUE;
		*d++=c;
		/* if (d > &gTempEnd-2) break; */
		c = *gSourcePtr++;
	}
	gSourcePtr--;
	if (d != gTempBuf) {
		*d++= 0;
		if (hasPoint) {
			r = OP_NUMBER_FLOAT;
			gLitFloat = (float)_atof(gTempBuf);
			
		} else 
		{
			r = OP_NUMBER_LONG;
			gLitLong = _atol(gTempBuf);
			if (gLitLong <= 256) 
			{
				gLitShort = (U8)gLitLong;
				r = OP_NUMBER_SHORT;
			} else
				if (gLitLong < 65536) 
				{
					gLitInt = (U16) gLitLong;
					r = OP_NUMBER_INT;
				}
		}
	}
	return r;

}

U8 _isalnum(U16 c)
{
	if (c >= 'a' && c <= 'z') return TRUE;
	if (c >= 'A' && c <= 'Z') return TRUE;
	if (c >= '0' && c <= '9') return TRUE;
	if (c == '$' || c == '!' || c == '.') return TRUE;
	return FALSE;
}

void UngetToken()
{
	gSourcePtr = gOldSourcePtr;
	if (gLastToken == OP_NEWLINE) 
		gLine--;
}

U8 GetToken()
{
	U8 r;
	U8 c;
	U8 *d,*p;
	BasicToken *tok = gBasToken;
	gLastTokenPtr = NULL;

	gOldSourcePtr = gSourcePtr;

	while ((c=*gSourcePtr++) != 0)
		if (!_isspace(c)) break;
	if (c == 0)
	{
		return OP_EOF;
	}


/* In Basic a command ends with a new line, so we need a token for it */
/*
	if (c == 10) {
		if (*gSourcePtr  == 13) gSourcePtr++;
		gLine ++;
		gLastToken = r;
		return OP_NEWLINE;
	}
*/
	if (c == 13) {
		if (*gSourcePtr  == 10) gSourcePtr++;
		gLine++;
		gLastToken = OP_NEWLINE;
		return OP_NEWLINE;
	}

	d = gTokenBuf;

/* do we have an operator or an string?  */

	r = _isop(c);
	if (r != 0) { gLastToken = r; return r; }

/* do we have a number?					 */
	if (c == '\'') {
		c = *gSourcePtr++;
		gLitLong = c;
		gLitShort = c;
		c =*gSourcePtr++;
		if (c != '\'') 
			gErrorCode = ERR_SYNTAX;
		return OP_NUMBER_SHORT;
	}

	r = _isnum(c);
	if (r != 0) { gLastToken = r; return r; }

	if (c == '"') /* We have a string */
	{
		d = gTempBuf;
		while ((c = *gSourcePtr++) != 0)
		{
			if (c == '"') break;
			*d++ = c;
		}
		*d++= 0;
		return OP_STRING;
	}

/* It now must be a variable or a keyword */

	d = gTempBuf;
	c = _toupper(c);
	gSourcePtr++;
	while (_isalnum(c))
	{
		if (c == 0)
			return OP_EOF;
		*d++=c;
/*		if (d > &gTempEnd-2) break; */
		c = _toupper(*gSourcePtr++);
	}
	gSourcePtr--;
	*d++ = 0;

/* check for keyword */

	tok = gBasToken;
	r = OP_VARIABLE;
	p = gTempBuf+strlen(gTempBuf)-1;
	c = *p;
	if (c == '$' || c == '!') *p = 0;

	while (tok->name != NULL)
	{
		if (strcmp(tok->name,gTempBuf) == 0) {
			r = tok->token;
			gLastTokenPtr = tok;
			
			break;
		}
		tok++;
	}
	*p = c;

/* for now, return variable */
	gLastToken = r;
    return r;
}

U8 Expect(U8 token)
{
	U8 tok;
	tok = GetToken();
	if (tok != token) {
		gErrorCode = ERR_SYNTAX;
		return 1;
	}
	return 0;
}

/* ---------------------------------------------------------------- */
/* Variable handling                                                */
/* ---------------------------------------------------------------- */

Variable *FindVariable(U8 *name);


Variable *CreateVariable2(U8 *name, U8 typ)
{
	Variable *r;
	U16      sv;
	U8 typsize;
	if (gPass == 2)
		gErrorCode = ERR_NEWVAR_IN_PASS2;

	r = &gVariables[gLastVar++];

	sv = sizeof(Variable);
	if (gLastVar > gSettings.Size_Variables / sizeof(Variable))
	{
		gErrorCode = ERR_OUT_OF_MEMORY;
	}

	r->m_Typ = typ;
	r->m_Name = ic_strdup(name);
	r->m_Offset = gVariablesSize;
	typsize = 0;
	switch(typ)
	{
		case TYP_8:  typsize = sizeof(U8);			break;
		case TYP_16: typsize = sizeof(U16);			break;
		case TYP_32: typsize = sizeof(U32);			break;
		case TYP_F:  typsize = sizeof(float);		break;
		case TYP_STR_C: typsize = sizeof(StrVar);		break;
		case TYP_NULL: typsize = 0;					break;
	}
	gVariablesSize += typsize;
	return r;
}

Variable *CreateVariable(U8 *name, U8 typ)
{
	Variable *r;
	r = FindVariable(name);
	if (r != NULL) return r;
	return CreateVariable2(name,typ);
}

void AllocVar(Variable *var, U8 typ)
{
	U8 typsize;
	U8 i;
	switch(typ)
	{
		case TYP_8:  typsize = sizeof(U8);			break;
		case TYP_16: typsize = sizeof(U16);			break;
		case TYP_32: typsize = sizeof(U32);			break;
		case TYP_F:  typsize = sizeof(float);		break;
		case TYP_STR_C: typsize = sizeof(ICString);	break;
		case TYP_NULL: typsize = 0;					break;
	}

	var->m_ElemSize = typsize;
	var->m_Size = typsize;
	for (i=0;i<var->cDim;i++)	
		var->m_Size *= var->m_Dim[i];
	var->m_Offset = gVariablesSize;
	gVariablesSize += var->m_Size;;
}

Variable *FindVariable(U8 *name)
{
	int i;
	U8 typ,*p;
	Variable *r;
	r = gVariables;
	for (i=0;i<gLastVar;i++)
	{
		if (strcmp(r->m_Name,name) == 0)
			return r;
		r++;
	}
/*	return NULL;  */
    p = name+strlen(name)-1;
	switch(*p)
	{
		case '$':	typ = TYP_STR_C;	break;
		case '!':	typ = TYP_16;		break;
		default:	typ = TYP_F;		break;
	}
	return CreateVariable2(name,typ);

	
}

/* ---------------------------------------------------------------- */
/* Expression                                                       */
/* ---------------------------------------------------------------- */


U8 CompileArrayIndex(Variable *var)
{
	U8 tok,i;
	U8 cDim;
	U8 typG = gExprType;
	U16 faktor;
	U8 adds;

	tok = GetToken();
	if (tok != OP_BOPEN) {
		UngetToken();
		return FALSE;
	}
	adds = 0;
	cDim = 0;
	while (TRUE)
	{
		gExprType = TYP_16;
		Expr(TYP_16);
// Check if we must multiply
		faktor = var->m_ElemSize;
		for (i = cDim+1;i<var->cDim;i++)
			faktor*= var->m_Dim[i];
		if (faktor != 1)
		{
			*gCodePtr				++= CMD_LOAD_C16;
			*((U16*)gCodePtr)		++= faktor;
			*gCodePtr				++= CMD_MULT + TYP_16;
		}
		adds++;

		cDim++;
		tok = GetToken();
		if (tok == OP_BCLOSE) break;
		if (tok != OP_KOMMA) {
			gErrorCode = ERR_SYNTAX;
			return FALSE;
		}
	}
	if (cDim != var->cDim)
	{
		gErrorCode = ERR_SYNTAX;
		return 0;
	}
	for (i=0;i<adds-1;i++)
		*gCodePtr++=CMD_ADD + TYP_16;
	gExprType = typG;
	return 1;
	

}

void UnwindString(U8 typ)
{
}

U8 IsStringTyp(U8 typ)
{
	return typ == TYP_STR_C || typ == TYP_STR_D || typ == TYP_STR_V;
}
/* Ensures that all stack entries are of the same size */ 

void WriteCast1(U8 FromTyp, U8 toTyp)
{
   switch(FromTyp)
   {
		case TYP_8: 
					switch(toTyp)
					{
						case TYP_16:
							*gCodePtr++ = CMD_EXT_8_16;
							break;
						case TYP_32:
							*gCodePtr++ = CMD_EXT_8_32;
							break;
						case TYP_F:
							*gCodePtr++ = CMD_EXT_8_F;
							break;

					}
					break;
		case TYP_16: 
					switch(toTyp)
					{
						case TYP_8:
							*gCodePtr++ = CMD_EXT_16_8;
							break;
						case TYP_32:
							*gCodePtr++ = CMD_EXT_16_32;
							break;
						case TYP_F:
							*gCodePtr++ = CMD_EXT_16_F;
							break;

					}
					break;
		case TYP_32: 
					switch(toTyp)
					{
						case TYP_8:
							*gCodePtr++ = CMD_EXT_32_8;
							break;
						case TYP_16:
							*gCodePtr++ = CMD_EXT_32_16;
							break;
						case TYP_F:
							*gCodePtr++ = CMD_EXT_32_F;
							break;

					}
					break;
		case TYP_F: 
					switch(toTyp)
					{
						case TYP_8:
							*gCodePtr++ = CMD_EXT_F_8;
							break;
						case TYP_16:
							*gCodePtr++ = CMD_EXT_F_16;
							break;
						case TYP_32:
							*gCodePtr++ = CMD_EXT_F_32;
							break;
					}
					break;


   }
}

void WriteCast(U8 typ)
{
	WriteCast1(typ,gExprType);
}
/* Pushes the last two types from the stack and makes them equal.
   We can only extend the top most stype, so the we must convert
   everything to the second type (until the VM has registers)
*/
   
U8 ExtendTypes()
{
	U8 typ1 = POP_TYPE;
	U8 typ2 = POP_TYPE;


	if (typ1 != typ2) {
		WriteCast1(typ1,typ2);
	}

	return typ2;
}

void Bracket()
{
	U8 tok;
	tok = GetToken();
	if (tok != OP_BOPEN) {
		gErrorCode = ERR_EXPECTED_BRACKET;
		return;
	}
	Expr(TYP_NULL);
	tok = GetToken();
	if (tok != OP_BCLOSE) {
		gErrorCode = ERR_EXPECTED_BRACKET;
		return;
	}
	PUSH_TYPE(gExprType);
}

void CompileSyntax(U8 *syntax)
{
	U8 *s;
	char c;
	s = syntax;
	while ((c = *s++) != 0)
	{
		switch(c)
		{
			case 'b': gExprType = TYP_8;
					  Expr(gExprType); 
					  if (IsStringTyp(gExprType)) gErrorCode = ERR_SYNTAX;
					  break;
			case 'i': gExprType = TYP_16; 
					  Expr(gExprType); 
					  if (IsStringTyp(gExprType)) gErrorCode = ERR_SYNTAX;
					  break;
			case 'l': gExprType = TYP_32; 
					  Expr(gExprType); 
					  if (IsStringTyp(gExprType)) gErrorCode = ERR_SYNTAX;
					  break;
			case 'f': gExprType = TYP_F; 
					  Expr(gExprType); 
					  if (IsStringTyp(gExprType)) gErrorCode = ERR_SYNTAX;
					  break;
			case 's': gExprType = TYP_F;  
					  Expr(TYP_NULL);  
					  if (!IsStringTyp(gExprType)) gErrorCode = ERR_SYNTAX;
					  break;
		}
		if (gErrorCode != 0) return;
		if (*s != 0)
			if (Expect(OP_KOMMA)) return;
	}
}

U8 CompileTableFunction(U8 token)
{

	U8 typ;
	BasicToken *tok;
	tok = gLastTokenPtr;
	if (tok == NULL)						return 0;
	if (tok->returntype == TYP_HANDLED || 
		tok->returntype == TYP_VOID)			return 0;

	if (tok->syntax != NULL)
	{
		typ = gExprType;
		if (Expect(OP_BOPEN)) return 0;
		CompileSyntax(tok->syntax);
		if (Expect(OP_BCLOSE)) return 0;
		gExprType = typ;
	}
	if (gErrorCode != 0) return 0;
	*gCodePtr++ = tok->op1;
	if (tok->op1 == CMD_API) *gCodePtr++ = tok->op2;
	PUSH_TYPE(tok->returntype);
	return 0;
}

U8 CompileFunction(U8 token)
{

	U8 typ;
	switch(token)
	{
		case OP_ABS:
			 Bracket();
			 typ = POP_TYPE;
			 if (!IsStringTyp(typ)) {
				 *gCodePtr++= CMD_ABS + typ;
				 PUSH_TYPE(typ);
			 }
			 else
				 gErrorCode = ERR_SYNTAX;
			 break;
		default:
			return CompileTableFunction(token);
	}
	return 0;
}
U8 Expr1()
{
	U8 token,index;
	U8 r;
/*	float  *cpf; */
	U8 op,typ;
	Variable *var;

	r = 0;


	token = GetToken();
	switch(token) 
	{
		case OP_SHORT:
			 typ = gExprType;
			 gExprType = TYP_8;
			 Bracket();
			 /* WriteCast1(gExprType,typ); */
			 gExprType = typ;
			 break;
		case OP_INTEGER:
			 typ = gExprType;
			 gExprType = TYP_16;
			 Bracket();
			 /* WriteCast1(gExprType,typ); */
			 gExprType = typ;
			 break;
		case OP_LONG:
			 typ = gExprType;
			 gExprType = TYP_32;
			 Bracket();
			 /*WriteCast1(gExprType,typ); */
			 gExprType = typ;
			 break;
		case OP_FLOAT:
			 typ = gExprType;
			 gExprType = TYP_F;
			 Bracket();
			 /* WriteCast1(gExprType,typ); */
			 gExprType = typ;
			 break;
		case OP_VARIABLE:
			 var = FindVariable(gTempBuf);
			 if (var == NULL)
			 {
				 gErrorCode = ERR_VAR_NOT_DEF;
				 return 0;
			 }
			 index = CompileArrayIndex(var);
			 if (!IsStringTyp(var->m_Typ))
			 {
				 if (index) index = ADR_INDEX;
				 if (gExprType == TYP_NULL)
					 gExprType = var->m_Typ;
				 op = CMD_LOAD + ADR_V + var->m_Typ + index;
				 *gCodePtr		  ++= op;
				 *((U16*)gCodePtr)++= (U16)var->m_Offset;
				 if (gExprType != TYP_NULL) {
					 WriteCast(var->m_Typ);
					 PUSH_TYPE(gExprType);
				 } else
				    PUSH_TYPE(var->m_Typ);				 
			 } else
			 {
				 if (index)
					*gCodePtr		  ++= CMD_LOAD_STR_VI;
				 else
				    *gCodePtr		  ++= CMD_LOAD_STR_V;
				 *((U16*)gCodePtr)    ++= (U16)var->m_Offset;
				 PUSH_TYPE(TYP_STR_V);
			 }
			 break;
	
		case OP_STRING:
			 *gCodePtr++ = CMD_LOAD_STR_C;
			 *((U16*)gCodePtr)++ = strlen(gTempBuf)+1;
			 strcpy(gCodePtr,gTempBuf);
			 gCodePtr+=strlen(gTempBuf);
			 *gCodePtr++=0;
/*			 s = gTempBuf;
			 d = gCodePtr;
			 while ((c = *s++) != 0)
				 *gCodePtr++ = c;
			 *gCodePtr++ = 0;
*/
			 PUSH_TYPE(TYP_STR_C);
			 break;

		case OP_GETKEY:
			 *gCodePtr++ = CMD_GETKEY;
			 WriteCast1(TYP_16,gExprType);
			 PUSH_TYPE(gExprType);
			 break;
		case OP_NUMBER_FLOAT:
				*gCodePtr++ = CMD_LOAD_CF;
				*((float*)gCodePtr)++ = gLitFloat;
				PUSH_TYPE(TYP_F);
				break;

		case OP_NUMBER_SHORT:
		case OP_NUMBER_INT:
		case OP_NUMBER_LONG:

			 switch(gExprType)
			 {
				case TYP_8: 
					*gCodePtr++ = CMD_LOAD_C8;
					*gCodePtr++ = (U8)gLitLong;
					PUSH_TYPE(TYP_8);
					break;

				case TYP_16:
					*gCodePtr++ = CMD_LOAD_C16;
					*((U16*)gCodePtr)++ = (U16)gLitLong;
					PUSH_TYPE(TYP_16);
					break;

				case TYP_32:
					*gCodePtr++ = CMD_LOAD_C32;
					*((U32*)gCodePtr)++ = (U32)gLitLong;
					PUSH_TYPE(TYP_32);
					break;

				case TYP_F:
					*gCodePtr++ = CMD_LOAD_CF;
					*((float*)gCodePtr)++ = (float)gLitLong;
					//WriteCast1(TYP_32,TYP_F);
					PUSH_TYPE(TYP_F);
					break;
			 }

			 r = token;
			 break;

		case OP_BOPEN:
			 r = Expr(TYP_NULL);
			 PUSH_TYPE(gExprType);
			 token = GetToken();
			 if (token != OP_BCLOSE) 
				 gErrorCode = ERR_SYNTAX;
			 break;

		default:
			 CompileFunction(token);
			 r = token;

	}
	return r;
}

U8 Expr2()
{
	U8 r;
	U8 valid = TRUE;
	U8 token;
	U8 typ;
	token = Expr1();
	if (token == OP_NEWLINE) return token;
	while (valid)
	{
		token = GetToken();
		switch(token) 
		{
			case OP_MULT:	if (!PeekTypIsString()) {
								r = Expr1();
								typ = ExtendTypes();
								*gCodePtr++ = CMD_MULT + typ;
								PUSH_TYPE(typ);
							}
							break;
			case OP_DIV:	if (!PeekTypIsString()) {
								r = Expr1();
								typ = ExtendTypes();
								*gCodePtr++ = CMD_DIV + typ;
								PUSH_TYPE(typ);
							}
							break;
			default:
				 r = token;
				 UngetToken();
				 valid = FALSE;
				 break;
		}
	}
	return r;
}
U8 Expr3()
{
	U8 r;
	U8 valid = TRUE;
	U8 token;
	U8 typ,typ2;
	Expr2();

	while (valid)
	{
		token = GetToken();
		switch(token)
		{
				case OP_ADD:	
								r = Expr0();	
								typ = POP_TYPE;
								if (IsStringTyp(typ))
								{
									typ2 = POP_TYPE;
									if (!IsStringTyp(typ2)) gErrorCode = ERR_SYNTAX;
									*gCodePtr++ = CMD_STRINGADD;
									PUSH_TYPE(TYP_STR_D);
								}
								else
								{
									PUSH_TYPE(typ);
									typ = ExtendTypes();
									*gCodePtr++ = CMD_ADD + typ;
									PUSH_TYPE(typ);
								}
								break;
				case OP_SUB:	if (!PeekTypIsString()) {
									r = Expr0();		
									typ = ExtendTypes();
									*gCodePtr++ = CMD_SUB + typ;	
									PUSH_TYPE(typ);
								}
								break;
				default:		r = token;
								valid = FALSE;
								UngetToken();
		}
	}
	return r;
	
}

/* 
expr4 ::= expr3 (( '>' expr3) | ( '<' expr3) | ( '>=' expr3) | ( '<=' expr3) | ( '=' expr3) 
expr  ::= ('NOT' expr4) | (expr4 (('AND' expr4) | ( 'OR' expr4))) 
*/

void WriteBoolRest()
{
	U16 *l1, *l2;
	U16 base = gCodePtr-gCode-1;
	l1 = (U16*)gCodePtr;
	return ;
	*((U16*)gCodePtr)++ = 0;
	*gCodePtr++ = CMD_LOAD + TYP_8;
	*gCodePtr++ = 0;
	PUSH_TYPE(TYP_8);
	/*
	switch(gExprType)
	{
		case TYP_8:		*gCodePtr ++=  CMD_LOAD + TYP_8;
						*gCodePtr ++=  0;
						break;
		case TYP_16:	*gCodePtr ++=  CMD_LOAD + TYP_16;
						*((U16*)gCodePtr)++=  0;
						break;
		case TYP_32:	*gCodePtr ++=  CMD_LOAD + TYP_32;
						*((U32*)gCodePtr)++=  0;
						break;
		case TYP_F:		*gCodePtr ++=  CMD_LOAD + TYP_F;
						*((float*)gCodePtr)++=  0;
						break;
	}
	*/
	*gCodePtr++= CMD_JUMP;
	l2 = (U16*)gCodePtr;
	*((U16*)gCodePtr)++ = 0;
	*l1 = gCodePtr-gCode;
	*gCodePtr++ = CMD_LOAD +TYP_8;
	*gCodePtr++ = 1;

	/*
	switch(gExprType)
	{
		case TYP_8:		*gCodePtr ++=  CMD_LOAD + TYP_8;
						*gCodePtr ++=  1;
						break;
		case TYP_16:	*gCodePtr ++=  CMD_LOAD + TYP_16;
						*((U16*)gCodePtr)++=  1;
						break;
		case TYP_32:	*gCodePtr ++=  CMD_LOAD + TYP_32;
						*((U32*)gCodePtr)++=  1;
						break;
		case TYP_F:		*gCodePtr ++=  CMD_LOAD + TYP_F;
						*((float*)gCodePtr)++=  1;
						break;
	}
	*/
	*l2 = gCodePtr-gCode;
}

void CompileCompare(U8 op)
{
	U8 r,typ,typ2;
	r = Expr0();		
	typ = POP_TYPE;
	if (IsStringTyp(typ)) {
		typ2 = POP_TYPE;
		if (!IsStringTyp(typ2)) {gErrorCode = ERR_SYNTAX; return; }
		
		*gCodePtr++= CMD_STRINGCMP;
		gCompareOp = gCodePtr;
		*gCodePtr++= op;
		PUSH_TYPE(TYP_8);
	}
	else
	{
		PUSH_TYPE(typ);
		typ = ExtendTypes();
		*gCodePtr++ = CMD_CMP + typ;
		gCompareOp = gCodePtr;
		*gCodePtr++ = op;
		PUSH_TYPE(TYP_8);
	}
}

U8 Expr4()
{
	U8 r;
	U8 valid = TRUE;
	U8 token;
	token = Expr3();
	while (valid)
	{
		token = GetToken();
		switch(token)
		{
				case OP_BIGGER:			CompileCompare(CMD_LOADF_LT);
										break;
				case OP_BIGGEREQUAL:	CompileCompare(CMD_LOADF_LE);
										PUSH_TYPE(TYP_8);
										break;
				case OP_SMALLER:		CompileCompare(CMD_LOADF_GT);
										PUSH_TYPE(TYP_8);
										break;
				case OP_SMALLEREQUAL:	CompileCompare(CMD_LOADF_GE);
										PUSH_TYPE(TYP_8);
										break;
				case OP_EQUAL:			CompileCompare(CMD_LOADF_Z);
										PUSH_TYPE(TYP_8);
										break;
				case OP_UNEQUAL:		CompileCompare(CMD_LOADF_NZ);
										PUSH_TYPE(TYP_8);
										break;
				default:		r = token;
								valid = FALSE;
								UngetToken();
		}
	}
	return r;
}


U8 Expr0()
{
	U8 r;
	U8 valid = TRUE;
	U8 token,typ;
	token = GetToken();
	if (token == OP_NOT) {
		Expr4();
		*gCodePtr += CMD_NOT + gExprType;
	} else
	{
		UngetToken();
		token = Expr4();
		while (valid)
		{
			token = GetToken();
			switch(token)
			{
				case OP_AND:		r = Expr0();
									typ = ExtendTypes();
									*gCodePtr++ = CMD_AND + typ;
									PUSH_TYPE(typ);
									break;

				case OP_OR:		    r = Expr0();
									typ = ExtendTypes();
									*gCodePtr++ = CMD_OR + typ;
									PUSH_TYPE(typ);
									break;
				default:			r = token;
									valid = FALSE;
									UngetToken();
			}
		}
	}
	return r;

}

U8 Expr(U8 ResultType)
{
	U8 r = 0;
	U8 typ;
	Expr0();
	typ = POP_TYPE;

	if (!IsStringTyp(typ)) {
		if (ResultType == TYP_NULL)
			WriteCast(typ);
		else
			WriteCast1(typ,ResultType);
	}
	else
		gExprType = typ;
	return r;
}

/* syntax: Dim variable as typ, [var as typ].. */
void CompileDim()
{
	U8 token;
	
	Variable *var;
	while (TRUE)
	{
		token = GetToken();
		if (token == OP_NEWLINE) break;
		if (token != OP_VARIABLE) {
			gErrorCode = ERR_EXPECED_VAR;
			return;
		}
		var = CreateVariable(gTempBuf,TYP_NULL);

		token = GetToken();
		if (token == OP_BOPEN)
		{
			var->cDim = -1;
			while (TRUE)
			{
				token = GetToken();
				if (token == OP_BCLOSE) break;
				if (token == OP_NUMBER_LONG || token == OP_NUMBER_SHORT || token == OP_NUMBER_INT)
				{
					var->m_Dim[++var->cDim] =(U16)gLitLong;
					token = GetToken();
					if (token == OP_BCLOSE) break;
					if (token != OP_KOMMA) {
						gErrorCode = ERR_SYNTAX;
						return;
					}
					if (var->cDim >= 3) {
						gErrorCode = ERR_SYNTAX;
						return;
					}
				}
				
				

			}
			token = GetToken();
			var->cDim++;
		}
		if (token != OP_AS)
		{
			gErrorCode = ERR_EXPECED_AS;
			return;
		}

		token = GetToken();

		switch(token)
		{
			case OP_STRING:
				 AllocVar(var,TYP_STR_C);
				 var->m_Typ = TYP_STR_C;
				 break;
			case OP_LONG:
				 AllocVar(var,TYP_32);
				 var->m_Typ = TYP_32;
				 break;
			case OP_FLOAT:
				 AllocVar(var,TYP_F);
				 var->m_Typ = TYP_F;
				 break;
			case OP_INTEGER:
				 AllocVar(var,TYP_16);
				 var->m_Typ = TYP_16;
				 break;
			case OP_SHORT:
				 AllocVar(var,TYP_8);
				 var->m_Typ = TYP_8;
				 break;
			default:
				 gErrorCode = ERR_EXPECED_TYPE;
				 return;
		}

	}
	gErrorCode = 0;
}

/* Compiles the print statement */
void CompileInput()
{
	U8 token;
	Variable *var;
	InsertDebugLine();
	
	token = GetToken();
	if (token == OP_STRING)
	{
		 *gCodePtr++=CMD_LOAD_STR_C;
		 *((U16*)gCodePtr)++ = strlen(gTempBuf)+1;
			 strcpy(gCodePtr,gTempBuf);
			 gCodePtr+=strlen(gTempBuf);
			 *gCodePtr++=0;
/*
		 s = gTempBuf;
		 d = gCodePtr;
		 while ((c = *s++) != 0)
			 *gCodePtr++ = c;
		 *gCodePtr++ = 0;
*/
		 *gCodePtr++ = CMD_PRINT_STR;
		 token = GetToken();
		 if (token != OP_SEMIKOLON)
		 {
			 gErrorCode = ERR_SYNTAX;
			 return;
		 }
		 token = GetToken();
	}
	
	if (token != OP_VARIABLE)
	{
		gErrorCode = ERR_SYNTAX;
		return;
	}
	var = FindVariable(gTempBuf);
	if (var == NULL)
	{
		gErrorCode = ERR_VAR_NOT_DEF;
		return;
	}
	switch(var->m_Typ)
	{
		case TYP_8:		*gCodePtr++ = CMD_INPUTI + TYP_8;			
						*gCodePtr++ = CMD_STORE_V8;
						*((U16*)gCodePtr)++ = (U16)var->m_Offset;
						break;
		case TYP_16:	*gCodePtr++ = CMD_INPUTI + TYP_16;			
						*gCodePtr++ = CMD_STORE_V16;
						*((U16*)gCodePtr)++ = (U16)var->m_Offset;
						break;
		case TYP_32:	*gCodePtr++ = CMD_INPUTI + TYP_32;			
						*gCodePtr++ = CMD_STORE_V32;
						*((U16*)gCodePtr)++ = (U16)var->m_Offset;
						break;
		case TYP_F:		*gCodePtr++ = CMD_INPUTI + TYP_F;			
						*gCodePtr++ = CMD_STORE_VF;
						*((U16*)gCodePtr)++ = (U16)var->m_Offset;
						break;
		case TYP_STR_C:	*gCodePtr++ = CMD_INPUT_STR;				
						*gCodePtr++ = CMD_STORE_STR_V;
						*((U16*)gCodePtr)++ = (U16)var->m_Offset;
						break;
	}
}

void CompilePrint()
{
	U8 token;
	U8 first= TRUE;
	InsertDebugLine();
	while (TRUE)
	{
		gExprType = TYP_F;
		token = GetToken();
		if (token == OP_NEWLINE) 
		{
			if (first) *gCodePtr++ = CMD_PRINT_NL;
			return;
		}
		first = FALSE;
		UngetToken();
		token = Expr(TYP_NULL);
		if (IsStringTyp(gExprType)) 
		{
			*gCodePtr++ = CMD_PRINT_STR;
			UnwindString(gExprType);
		}
		else
			*gCodePtr++ = CMD_PRINTI + gExprType;

		token = GetToken();
		switch(token)
		{
				case OP_KOMMA:  *gCodePtr++ = CMD_PRINT_TAB;
							    break;
				case OP_SEMIKOLON:
								break;
				case OP_NEWLINE:
					*gCodePtr++ = CMD_PRINT_NL;
					return;
				default: 
					UngetToken();
					return;
		}
	}
}

/* Compiles the let statement or a label */
/*
    stmt := var '=' expr				; Assignment
	stmt := var ':'						; Label
*/
void CompileLet()
{
	Variable *var;
	U8 tok;
	U8 index;
	
	InsertDebugLine();

/* If we have a label, the next token is a colon */

	tok = GetToken();
	if (tok == OP_COLON) {
		var = CreateVariable(gTempBuf,TYP_LABEL);
		var->m_Offset = gCodePtr-gCode;
		return;
	}
	
	else
	{
		UngetToken();

		var = FindVariable(gTempBuf);
		
		if (var == NULL) {
			gErrorCode = ERR_VAR_NOT_DEF;
			return;
		}
		index = CompileArrayIndex(var);

		if (var->m_Typ == TYP_LABEL) 
		{
			gErrorCode = ERR_NOT_A_VAR;
			return;
		}

		tok = GetToken();
		if (tok != OP_EQUAL) {
			gErrorCode= ERR_EXPECED_EQUAL;
			return;
		}

		gExprType  = var->m_Typ;
		Expr(TYP_NULL);

		
		if (!IsStringTyp(gExprType)) {						/* Number assignment */
			if (index) index = ADR_INDEX;
			*gCodePtr++ = CMD_STORE + ADR_V + gExprType + index;
			*((U16*)gCodePtr)++ = (U16)var->m_Offset;
		} else												/* Stirng Assignment */
		{
			if (index)
				*gCodePtr++ = CMD_STORE_STR_VI;
			else
				*gCodePtr++ = CMD_STORE_STR_V;

			*((U16*)gCodePtr)++ = (U16)var->m_Offset;
		}

	}

	/* !! Add string type here */
}
/* A for loop loops at least one time ! */

/* A for loop consists of the following code:

       Expr						; Do an expression for FROM 
	   STORE V variable			; Store the variable
	   Expr						; Do an expression for TO 
	   JMP   for_compare

    for_loop:
       Code

    for_compare:
	   
	   DUP    typ				; We need this value on every loop, so dup it on stack
	   LOAD   v Variable		; Load the variable for compare
	   CMP	  typ				; Make the compare on the type
	   JLE    for_loop			; Jump if less or equal then
	   ADDSP  sizeof(typ)		; Drop the TO value from the stack

*/

U8 CompileUntil(U8 op1, U8 op2)
{
	U8 tok;
	while (TRUE)
	{
		tok = GetToken();
		if (tok == op1 || tok == op2 || tok == OP_EOF) return tok;
		UngetToken();
		
		tok = CompileStatement();
		
		if (gErrorCode != 0) return 0;
	}


}

/* Syntax: while expr statements wend */
void CompileWhile()
{

	U16   *adrJumpLoop;
	U8    *startAdr;
	
	
	InsertDebugLine();

	
	startAdr = gCodePtr;
	gExprType = TYP_NULL;
	Expr(TYP_8);
	if (gErrorCode != 0) return; 

	/* Make the same as compiling an if expr = 0 goto exit*/
	*gCodePtr ++=   CMD_LOAD_C8;
	*gCodePtr ++=   0;
	*gCodePtr ++=	CMD_CMP + TYP_8;
	*gCodePtr ++=	CMD_JUMP + COND_Z;
	adrJumpLoop = (U16*)gCodePtr;
	*((U16*)gCodePtr)++ = 0;

	CompileUntil(OP_WEND,0);
	*gCodePtr ++= CMD_JUMP;
	*((U16*)gCodePtr)++ = startAdr-gCode;
	*adrJumpLoop = gCodePtr-gCode;
}

void CompileFor()
{
	U8 tok,tok2,o;
	Variable *var;
	U8 op;
	U16 codeStart;
	U16 *codeExitJump;



	InsertDebugLine();
	tok = GetToken();
	if (tok != OP_VARIABLE) {
		gErrorCode = ERR_EXPECED_VAR;
		return;
	}

	var = FindVariable(gTempBuf);
	if (var == NULL) {
		gErrorCode = ERR_VAR_NOT_DEF;
		return;
	}

	tok = GetToken();
	if (tok != OP_EQUAL) {
		gErrorCode = ERR_EXPECED_EQUAL;
		return;
	}
/* Store the value FROM into a variable and keep track of the variable */
	gExprType = var->m_Typ;
	Expr(TYP_NULL);
	if (gErrorCode != 0) return;

    if (!IsStringTyp(var->m_Typ))
	{
	  op = CMD_STORE + ADR_V + var->m_Typ;
	  *gCodePtr		  ++= op;
	  *((U16*)gCodePtr)++= (U16)var->m_Offset;
	  WriteCast(var->m_Typ);
	}

	tok = GetToken();
	if (tok != OP_TO) {
		gErrorCode = ERR_EXPECED_TO;
		return;
	}

/* Push the to value on the stack */ 

	Expr(TYP_NULL);
	if (gErrorCode != 0) return;


	*gCodePtr++ = CMD_JUMP;					/* Jump to the compare routine in the stack */
	codeExitJump = (U16*)gCodePtr;
	*((U16*)gCodePtr)++ = 0;		
	codeStart = gCodePtr-gCode;

/* Compile until next or exit for */

	CompileUntil(OP_NEXT,0);

/* compile the next					*/

	InsertDebugLine();

	tok2 = GetToken();
	if (tok2 != OP_VARIABLE)
	{
		UngetToken();
	}
	else
	{
		/* then the variable must match */
		if (strcmp(gTempBuf,var->m_Name) != 0) {
			/* Msg1(gTempBuf); */
			/* gErrorCode = ERR_VAR_NOT_MATCH;
			return; */
		}
	}
	

	gExprType = var->m_Typ;
	*gCodePtr			++= CMD_INC;
	*gCodePtr			++= ADR_V + var->m_Typ;
	*((U16*)gCodePtr)   ++= (U16)var->m_Offset;
	/*
	*gCodePtr			++= CMD_LOAD + ADR_V + var->m_Typ;
	*((U16*)gCodePtr)   ++= (U16)var->m_Offset;
	*gCodePtr			++= CMD_LOAD + ADR_C;
	*gCodePtr		    ++= 1;
	WriteCast(TYP_8);
	*gCodePtr			++= CMD_ADD + var->m_Typ;
	*gCodePtr			++= CMD_STORE + ADR_V + var->m_Typ;
	*((U16*)gCodePtr)   ++= (U16)var->m_Offset;
    */
	/* Do the compare */
	

	*codeExitJump		= gCodePtr-gCode;
	*gCodePtr			++= CMD_DUP + var->m_Typ;
	*gCodePtr			++= CMD_LOAD + ADR_V + var->m_Typ;
	*((U16*)gCodePtr)	++= (U16)var->m_Offset;
	*gCodePtr			++= CMD_CMP + var->m_Typ;
	*gCodePtr			++= CMD_JUMP_LE;
	*((U16*)gCodePtr)   ++= codeStart;
	*gCodePtr			++= CMD_ADDSP;
	switch(var->m_Typ) {
			case TYP_8:   o = 1; break;
			case TYP_16:  o = 2; break;
			case TYP_32:  o = 4; break;
			case TYP_F :  o = sizeof(float); break;
	}
	*gCodePtr			++= o;


}


// simply skips until end of line
void CompileRem()
{
	U8 c;
	while ((c = *gSourcePtr++) != 0)
	{
		if (c == 10) {
			if (*gSourcePtr == 13) gSourcePtr++;
			break;
		}
		if (c == 13) {
			if (*gSourcePtr == 10) gSourcePtr++;
			gLine++;
			break;
		}
	}
}

void CompileGosub()
{
	U8 token;
	Variable *var;
	InsertDebugLine();
	token = GetToken();
	if (token != OP_VARIABLE) {
		gErrorCode = ERR_OP_NOT_A_LABEL;
		return;
	}
	var = FindVariable(gTempBuf);
	*gCodePtr		 ++= CMD_CALL;
	if (var != NULL)
	{
		*((U16*)gCodePtr)++= (U16)var->m_Offset;
	} else
	{
		if (gPass == 2) {
			gErrorCode = ERR_LABEL_NOT_DEFINED;
			return;
		}
		else
			*((U16*)gCodePtr)++= 0;
	}
}

U8 NegCond(U8 op)
{
	switch (op)
	{
		case COND_Z:	op = COND_NZ;		break;
		case COND_NZ:	op = COND_Z;		break;
		case COND_GT:	op = COND_LE;		break;
		case COND_LE:	op = COND_GT;		break;
		case COND_GE:	op = COND_LT;		break;
		case COND_LT:	op = COND_GE;		break;
	}
	return op;
}

void CompileIf()
{
	U8 *label1,*label2;
	U8 tok,op;
	InsertDebugLine();
	gExprType = TYP_NULL;
	Expr(TYP_8);
	if (gErrorCode != 0) return;

	if (gCodePtr- gCompareOp == 1)
	{
		op = *gCompareOp;
		*gCompareOp ++=	CMD_JUMP + (NegCond(op & 7));
	}
	else
	{
		*gCodePtr ++=   CMD_LOAD_C8;
		*gCodePtr ++=    0;
		*gCodePtr ++=	CMD_CMP + TYP_8;
		*gCodePtr ++=	CMD_JUMP + COND_Z;
	}
	label1 = gCodePtr;
	*((U16*)gCodePtr)++ = 0;

	tok = GetToken();
	if (tok != OP_THEN)
	{
		gErrorCode = ERR_SYNTAX;
		return;
	}

	/* Do we have a multi line if? */
	tok = GetToken();
	if (tok == OP_NEWLINE)
	{
		/* Compile a long if */
		tok = CompileUntil(OP_END,OP_ELSE);			
/*		tok = GetToken(); */
		switch(tok)
		{
			case OP_ELSE:
					 *gCodePtr++ = CMD_JUMP;
					 label2 = gCodePtr;
					 *((U16*)gCodePtr)++ = 0;
					 *((U16*)label1) = gCodePtr-gCode;
					 label1 = label2;
					 tok = CompileUntil(OP_END,0);
					 if (Expect(OP_IF)) return;
					 *((U16*)label1) = gCodePtr-gCode;
					 gErrorCode = 0;
					 return;

			case OP_END:
				     gErrorCode = 0;
				     if (Expect(OP_IF)) return;
					 *((U16*)label1) = gCodePtr-gCode;
					 return;
		}
		return;
	}
	else
	{
		/* it is a single line, now we find statements and eventually
		   else
		*/
		/* if expr then statment1 else statement2 */
		while (TRUE)
		{
			switch(tok)
			{
				case OP_ELSE:
					 
					 *gCodePtr++ = CMD_JUMP;
					 label2 = gCodePtr;
					 *((U16*)gCodePtr)++ = 0;
					 *((U16*)label1) = gCodePtr-gCode;
					 label1 = label2;
					 break;
				case OP_COLON:
					 break;
				case OP_NEWLINE:
					 *((U16*)label1) = gCodePtr-gCode;
					 return;
				default:
					UngetToken();
					CompileStatements();
					if (gErrorCode != 0) return;
					break;
			}
			tok = GetToken();
		}
	}

}

void CompileGoto()
{
	U8 token;
	Variable *var;
	InsertDebugLine();
	token = GetToken();
	if (token != OP_VARIABLE) {
		gErrorCode = ERR_OP_NOT_A_LABEL;
		return;
	}
	*gCodePtr		 ++= CMD_JUMP;
	var = FindVariable(gTempBuf);
	if (var != NULL)
	{
		*((U16*)gCodePtr)++= (U16)var->m_Offset;
	} else
	{
		if (gPass == 2) {
			gErrorCode = ERR_LABEL_NOT_DEFINED;
			return;
		}
		else
			*((U16*)gCodePtr)++= 0;
	}
}

void CompileReturn()
{
	InsertDebugLine();
	*gCodePtr++ = CMD_RTS;
}

// Compiles DOT expr,expr
// draws a point
void Compiler2U8()
{

}

U8 CompileCommandViaTable(U8 token)
{
	BasicToken *tok = gLastTokenPtr;
	if (tok == NULL)						return FALSE;
	if (tok->returntype != TYP_VOID )      {
			gErrorCode = ERR_FUNCTION_AS_STATEMENT; 
			return FALSE;
	}
	InsertDebugLine();
	if (tok->syntax != NULL)
		CompileSyntax(tok->syntax);
	*gCodePtr++ = tok->op1;
	if (tok->op1 == CMD_API) *gCodePtr++ = tok->op2;
	return TRUE;

/*
	CompileToken *tok = gCompileToken;
	U8 *s,c;
	while (tok->token != 0)
	{
		if (tok->token == token)
		{
			InsertDebugLine();
			CompileSyntax(tok->Syntax);
			*gCodePtr++ = tok->op1;
			if (tok->op1 == CMD_API) *gCodePtr++ = tok->op2;
			return 0;
		}
		tok++;
	}
	return 0;
*/
}


U8 CompileStatement()
{
	U8 token;

	gLastSourcePtr = gSourcePtr;

	
	if (gCompilerShowListing) {
		ShowLine(gSourcePtr);
	}
	
	token = GetToken();
	if (token == OP_EOF) {
		gErrorCode = ERR_OK;
		return 1;
	}
	if (token == OP_ERROR) {
		gErrorCode = ERR_SYNTAX;
		return 1;
	}

//	itoa(token,gTempBuf,10);
	switch(token)
	{
		case OP_END: return token;
		/*
		case OP_END:		{
			 gErrorCode = ERR_END;
			 *gCodePtr  = CMD_HALT;					return token;
							}
		*/
		case OP_EOF:		
			 gErrorCode = ERR_END;
			 *gCodePtr  = CMD_HALT;					return token;
		case OP_DIM:		CompileDim();							break;
		case OP_PRINT:		CompilePrint();							break;
		case OP_INPUT:		CompileInput();							break;
		case OP_VARIABLE:	CompileLet();							break;
		case OP_NEWLINE:											break;
		case OP_FOR:		CompileFor();							break;
		case OP_WHILE:		CompileWhile();							break;
		case OP_REM:		CompileRem();							break;
		case OP_GOTO:		CompileGoto();							break;
		case OP_GOSUB:		CompileGosub();							break;
		case OP_RETURN:		CompileReturn();						break;
		case OP_IF:			CompileIf();							break;
		case OP_CLS:		
			InsertDebugLine();
			*gCodePtr++ = CMD_CLS;					
			break;
		default:
			if (!CompileCommandViaTable(token)) 
				return token;
	}
	return token;

}

void CompileStatements()
{
	U8 tok;
	while (TRUE)
	{
		CompileStatement();
		tok = GetToken();
		if (tok != OP_COLON)
		{
			UngetToken();
			break;
		}
	}
}


void ReportError()
{

	if (gErrorCode != 0) {
		if (gErrorCode < MAX_ERROR) {
			_print("Line ");
			_printi(gLine);
			_print(" ");
			
			ShowLine(gLastSourcePtr);
			_print("Error ");
			_printi(gErrorCode);
			_print(" ");
			_print(Errors[gErrorCode]);
		}
		else {
			ShowLine(gLastSourcePtr);
			_print(__errUnexp);
		}
	} else
		gErrorCode = 0;

}

void Compile()
{
	CodeHeader *codeHeader;
	gErrorCode = 0;
	gVariablesSize = 0;			/* Size of variable space, 0 Variables so far */
	gLine = 1;
	codeHeader = (CodeHeader*)gCode;
	memset(codeHeader,0,sizeof(CodeHeader));	
	gCodePtr = gCode+sizeof(CodeHeader);;
	gSourcePtr = gSource;
	gStackPtr = gStack+gSettings.Size_Stack;
	while (1)
	{
		if (gErrorCode != 0) 	  	 break;
		if (CompileStatement() == 1) break;
	}
	codeHeader->m_SizeCode = gCodePtr-gCode;
	codeHeader->m_SizeVariables = gVariablesSize;	
}


void InitCompiler()
{
	memset(gVariables,0,gSettings.Size_Variables);
	memset(gHeap,0,gSettings.Size_Heap);

}

void GetAPIOpName(U8 *target, U8 apiOpCode)
{
	BasicToken   *tok;

	tok = gBasToken;
	while(tok->token != 0)
	{
		if (tok->op1 == CMD_API && tok->op2 == apiOpCode)
		{
			strcpy(target,tok->name);
			break;
		}
		tok++;
	}
}

void CommandNameFromOpCode(U8 *target, U8 opcode)
{
	BasicToken   *tok;

	tok = gBasToken;
	while(tok->token != 0)
	{
		if (tok->op1 == opcode)
		{
			strcpy(target,tok->name);
			break;
		}
		tok++;
	}
}