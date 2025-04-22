
#include "ic35sdk.h"
#include "data.h"
#include "compiler.h"
#include "compapi.h"
#include "vm.h"
#include "vmapi.h"

/* The main token table contains all tokens by names, the syntax, types and a help.
   Some tokens are handled by a separate function in compiler.c or comapi.c, such as if and for.
   Most tokens have a fixed syntax (like dot or gotoxy) or functions such as mid. The code for parsing
   them is always the same, so we use a letter for type specifications:
		b = TYP_8
		i = TYP_16
		l = TYP_32
		f = TYP_F
		s = TYP_STR
*/

BasicToken gBasToken[] = {
	{"PRINT",			OP_PRINT			,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"INPUT",			OP_INPUT			,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"DIM",				OP_DIM				,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"AS",				OP_AS				,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"IF",				OP_IF				,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"THEN",			OP_THEN				,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"ELSE",			OP_ELSE				,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"END",				OP_END				,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"INTEGER",			OP_INTEGER			,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"LONG",			OP_LONG				,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"SHORT",			OP_SHORT			,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"BYTE",			OP_SHORT			,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"STRING",			OP_STRING			,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"FLOAT",			OP_FLOAT			,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"FOR",				OP_FOR				,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"TO",				OP_TO				,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"NEXT",			OP_NEXT				,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"WHILE",			OP_WHILE			,TYP_HANDLED,		0,	     0,					NULL,	  NULL},
	{"WEND",			OP_WEND				,TYP_HANDLED,		0,	     0,					NULL,	  NULL},
	{"GOTO"	,			OP_GOTO				,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"GOSUB",			OP_GOSUB			,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"REM",				OP_REM				,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"NOT",				OP_NOT				,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"AND",				OP_AND				,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"ABS",				OP_ABS				,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"OR",				OP_OR				,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"RETURN",			OP_RETURN			,TYP_HANDLED,       0,       0,					NULL,     NULL},
	{"POKE",			OP_POKE				,TYP_VOID,	 CMD_POKE,       0,					"ib",     "address,value"},
	{"OUT",				OP_OUT				,TYP_VOID,	 CMD_OUT,        0,					"bb",     "port,value"},
	{"PEEK",			OP_PEEK				,TYP_VOID,	 CMD_PEEK,       0,					"i",      "address"},
	{"IN",				OP_IN				,TYP_VOID,	 CMD_IN,         0,					"b",      "port"},
	{"DOT",				OP_DOT				,TYP_VOID   ,CMD_PLOT,       0,					"bbb",    "color,x,y"},
	{"LINE",			OP_LINE				,TYP_VOID   ,CMD_LINE,       0,					"biiii",  "color,x,y,x1,y1"},
	{"CLS",				OP_CLS				,TYP_VOID   ,CMD_CLS,		 0,					NULL,     NULL},
	{"GOTOXY",			OP_GOTOXY			,TYP_VOID   ,CMD_GOTOXY,     0,					"bb",     "x,y"},
	{"GETKEY",			OP_GETKEY			,TYP_16		,CMD_GETKEY,     0,					NULL,     NULL},
	{"MID",				OP_MID				,TYP_STR_C  ,CMD_MID,		 0,					"sii",    "string,start,len"},
	{"LEFT",			OP_LEFT				,TYP_STR_C  ,CMD_LEFT,       0,					"si",     "string,len"},
	{"RIGHT",			OP_RIGHT			,TYP_STR_C  ,CMD_RIGHT,      0,					"si",     "string,len"},
	{"LEN",				OP_LEN				,TYP_16     ,CMD_LEN,        0,					"s",      "string"},
	{"ASC",				OP_ASC				,TYP_16     ,CMD_ASC,        0,					"s",      "string"},
	{"CHR",				OP_CHR				,TYP_STR_C  ,CMD_CHR,        0,					"i",      "number"},
	{"STR",				OP_STR				,TYP_STR_C  ,CMD_STR,        0,					"f",      "number"},
	{"VAL",				OP_VAL				,TYP_F	    ,CMD_VAL,        0,					"s",      "string"},
	{"RANDOMIZE",		OP_RANDOMIZE		,TYP_VOID   ,CMD_RANDOMIZE,  0,					NULL,     NULL},
	{"RAND",			OP_RAND				,TYP_F      ,CMD_RAND_F,     0,					NULL,     NULL},
	{"RND",				OP_RAND				,TYP_F      ,CMD_RAND_F,     0,					NULL,     NULL},
	{"SIN",				OP_SIN				,TYP_F      ,CMD_SIN,		 0,					"f",      "float"},
	{"COS",				OP_COS				,TYP_F      ,CMD_COS,        0,					"f",      "float"},
	{"TAN",				OP_TAN				,TYP_F      ,CMD_TAN,        0,					"f",      "float"},
	{"DATE",			OP_DATE				,TYP_F      ,CMD_DATE_S,     0,					NULL,     NULL},
	{"TIME",			OP_TIME				,TYP_F      ,CMD_TIME_S,     0,					NULL,     NULL},
	{"BREAKPOINT",		OP_BREAKPOINT		,TYP_VOID	,CMD_BREAKPOINT, 0,					"b",	  "1=BASIC,2=ASS"},
	{"HALT",			OP_HALT				,TYP_VOID	,CMD_HALT,		 0,					"",		  "Stops execution"},
	{"FILLBLOCK",		OP_FILLBLOCK		,TYP_VOID   ,CMD_API, CMD_API_FILLBLOCK,		"biiii",  "color,x,y,w,h"},
	{"FILLSCREEN",		OP_FILLSCREEN		,TYP_VOID   ,CMD_API, CMD_API_FILLSCREEN,		"b",      "color"},
	{"INVERSEBLOCK",	OP_INVERSEBLOCK		,TYP_VOID   ,CMD_API, CMD_API_INVERSEBLOCK,		"iiii",   "x,y,w,h"},
	{"VLINE",			OP_VLINE			,TYP_VOID   ,CMD_API, CMD_API_VLINE,			"biiib",  "c,x,y,y1,w,typ"},
	{"HLINE",			OP_HLINE			,TYP_VOID   ,CMD_API, CMD_API_HLINE,			"biiib",  "c,x,y,x1,w,typ"},
	{"RECTANGLE",		OP_RECTANGLE		,TYP_VOID   ,CMD_API, CMD_API_RECTANGLE,		"biiiib", "c,x,y,w"	},
	{"CURSORSTATUS",	OP_CURSORSTATUS		,TYP_VOID   ,CMD_API, CMD_API_CURSORSTATUS,		"b",      "hide/show"},
	{"CURSORSHAPE",		OP_CURSORSHAPE		,TYP_VOID   ,CMD_API, CMD_API_CURSORSHAPE,		"bi",     "width,height"},
	{"FONT",			OP_FONT				,TYP_VOID   ,CMD_API, CMD_API_FONT,				"b",      "font"},
	{"SCROLLUP",		OP_SCROLLUP			,TYP_VOID   ,CMD_API, CMD_API_SCROLLUP,			"iiiiib", "x,y,w,h,offset,color"},
	{"SCROLLDN",		OP_SCROLLDN			,TYP_VOID   ,CMD_API, CMD_API_SCROLLDN,			"iiiiib", "x,y,w,h,offset,color"},
	{"SCROLLLEFT",		OP_SCROLLLEFT		,TYP_VOID   ,CMD_API, CMD_API_SCROLLLEFT,		"iiiiib", "x,y,w,h,offset,color"},
	{"SCROLLRIGHT",		OP_SCROLLRIGHT		,TYP_VOID   ,CMD_API, CMD_API_SCROLLRIGHT,		"iiiiib", "x,y,w,h,offset,color"},
	{"RESTORELCD",		OP_RESTORELCD		,TYP_VOID   ,CMD_API, CMD_API_RESTORELCD,		"iiii",   "x,y,w,h"},
	{"BACKUPLCD",		OP_BACKUPLCD		,TYP_VOID   ,CMD_API, CMD_API_BACKUPLCD,		"iiii",   "x,y,w,h"},
	{"RESTOREHELPLCD",	OP_RESTOREHELPLCD	,TYP_VOID   ,CMD_API, CMD_API_RESTOREHELPLCD,	NULL,     NULL},
	{"BACKUPHELPLCD",	OP_BACKUPHELPLCD	,TYP_VOID   ,CMD_API, CMD_API_BACKUPHELPLCD,	NULL,     NULL},
	{"SETINDION",		OP_SETINDION		,TYP_VOID   ,CMD_API, CMD_API_SETINDION,		"b",      "indi"},
	{"SETINDIOFF",		OP_SETINDIOFF		,TYP_VOID   ,CMD_API, CMD_API_SETINDIOFF,		"b",      "indi"},
	{"SETTONE",			OP_SETTONE			,TYP_VOID   ,CMD_API, CMD_API_SETTONE,			"i",      "tone"},
	{"MAKEWARNTONE",	OP_MAKEWARNTONE		,TYP_VOID   ,CMD_API, CMD_API_MAKEWARNTONE,		NULL,     NULL},
	{"SETPOWERMODE",	OP_SETPOWERMODE		,TYP_VOID   ,CMD_API, CMD_API_SETPOWERMODE,		"b",      "mode"},
	{"MASKSYMKEY",		OP_MASKSYMKEY		,TYP_VOID   ,CMD_API, CMD_API_MASKSYMKEY,		"b",      "mode"},
	{"MASKFNTKEY",		OP_MASKFNTKEY		,TYP_VOID   ,CMD_API, CMD_API_MASKFNTKEY,		"b",      "mode"},
	{"CLEARCHARBUF",	OP_CLEARCHARBUF		,TYP_VOID   ,CMD_API, CMD_API_CLEARCHARBUF,		NULL,     NULL},
	{"SETKEYTONE",		OP_SETKEYTONE		,TYP_VOID   ,CMD_API, CMD_API_SETKEYTONE,		"b",      "tone"},
	{"STRINGMENU",		OP_STRINGMENU		,TYP_16     ,CMD_API, CMD_API_STRINGMENU,		"si",     "menu,typ"},
	{"VSCROLLBAR",		OP_VSCROLLBAR		,TYP_VOID   ,CMD_API, CMD_API_VSCROLLBAR,		"iiiiii", "total,sect,cur,x,y,height"},
	{"SHADOWEDGERECT",	OP_SHADOWEDGERECT	,TYP_VOID   ,CMD_API, CMD_API_SHADOWEDGERECT,	"iiii",   "x,y,w,h"},
	{"STRINGDIALOGBOX",	OP_STRINGDIALOGBOX	,TYP_16		,CMD_API, CMD_API_STRINGDIALOGBOX,  "si",     "oo,typ"},
	{"STRINGPHONEMENU",	OP_STRINGPHONEMENU	,TYP_16		,CMD_API, CMD_API_STRINGPHONEMENU,  "si",     "m,t"},
	{"BUTTON",			OP_BUTTON			,TYP_VOID   ,CMD_API, CMD_API_BUTTON,			"siii",   "text,x,y,typ"},
	{"CHECKBOX",		OP_CHECKBOX			,TYP_STR_C  ,CMD_API, CMD_API_CHECKBOX,			"ssii",   "opt,return,x,y"},
	{"RADIOBUTTON",		OP_RADIOBUTTON		,TYP_16		,CMD_API, CMD_API_RADIOBUTTON,		"siii",   "o,f,x,y,typ"},
	{"LOADIMAGE",		OP_LOADIMAGE		,TYP_32     ,CMD_API, CMD_API_LOADIMAGE,		"s",	  "Loads image without index"},
	{"ALLOCIMAGE",		OP_ALLOCIMAGE		,TYP_32     ,CMD_API, CMD_API_ALLOCIMAGE,		"i",	  "size"},
	{"PUTIMAGE",		OP_PUTIMAGE			,TYP_VOID   ,CMD_API, CMD_API_PUTIMAGE,		    "lbb",	  "adr,x,y"},
	{"GETIMAGE",		OP_GETIMAGE			,TYP_VOID   ,CMD_API, CMD_API_GETIMAGE,		    "lbbbb",  "adr,x,y,w,h "},
	{"SCANKEY",			OP_SCANKEY			,TYP_16		,CMD_SCANKEY,	0,					NULL,	  "0 if no key"},
	{"DELAY1MS",		OP_DELAY1MS			,TYP_VOID   ,CMD_API, CMD_API_DELAY1MS,			"i",	  "anz"},
	{"DELAY250MS",		OP_DELAY250MS		,TYP_16     ,CMD_API, CMD_API_DELAY250MS,		"i",	  "anz"},
	{"DELAY1S",		    OP_DELAY1S			,TYP_16     ,CMD_API, CMD_API_DELAY1S,			"i",	  "anz"},


	{NULL,				0,					 0,       0,	   0, NULL,     NULL}
};



