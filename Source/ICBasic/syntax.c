


#define OP_EOF					0
#define OP_ERROR				240
#define OP_NEWLINE				241
#define OP_VARIABLE				250
#define OP_NUMBER_INT			252
#define OP_NUMBER_LONG			253
#define OP_NUMBER_SHORT			254
#define OP_NUMBER_FLOAT			255

#define OP_ADD					1
#define OP_SUB					2
#define OP_MULT					3
#define OP_DIV					4
#define OP_EQUAL				5
#define OP_BOPEN				6
#define OP_BCLOSE				7
#define OP_COLON				8
#define OP_BIGGER				9
#define OP_SMALLER				10	
#define OP_BIGGEREQUAL			11
#define OP_SMALLEREQUAL			12
#define OP_POWER				13
#define OP_SEMIKOLON			14
#define OP_KOMMA				15
#define OP_OR					16
#define OP_NOT					17
#define OP_AND					18
#define OP_UNEQUAL				19

#define OP_DIM					20
#define OP_AS					21
#define OP_BYTE					22
#define OP_STRING				23
#define OP_LONG					24
#define OP_FLOAT				25
#define OP_INTEGER				26
#define OP_SHORT				27

#define OP_LET					30
#define OP_IF					31
#define OP_THEN					32
#define OP_ELSE					33
#define OP_END					34
#define OP_FOR					35
#define OP_NEXT					36
#define OP_WHILE				37
#define OP_WEND					38
#define OP_DO					39
#define OP_LOOP					40
#define OP_EXIT					41
#define OP_TO					43
#define OP_GOTO					45
#define OP_GOSUB				46
#define OP_RETURN				47



#define OP_PRINT				50
#define OP_INPUT				51
#define OP_GOTOXY				52
#define OP_DOT 					53
#define OP_LINE					54
#define OP_CLS					55
#define OP_REM					56
#define OP_MID					57
#define OP_LEFT					58
#define OP_RIGHT				59
#define OP_LEN					60
#define OP_ASC					70
#define OP_CHR					71
#define OP_STR					72
#define OP_VAL					73
#define OP_RANDOMIZE			74
#define OP_RAND					75

#define OP_SIN					90
#define OP_COS					91
#define OP_TAN					92
#define OP_DATE					93
#define OP_TIME					94

#define   OP_FILLBLOCK				150
#define   OP_FILLSCREEN				151
#define   OP_INVERSEBLOCK			152
#define   OP_VLINE					153
#define   OP_HLINE					154
#define   OP_RECTANGLE				155
#define   OP_CURSORSTATUS			156
#define   OP_CURSORSHAPE			157
#define   OP_FONT					158
#define   OP_SCROLLUP				159	
#define   OP_SCROLLDN				160
#define   OP_SCROLLLEFT				161	
#define   OP_SCROLLRIGHT			162	
#define   OP_RESTORELCD				163	
#define   OP_BACKUPLCD				164
#define   OP_RESTOREHELPLCD			165
#define   OP_BACKUPHELPLCD			166
#define   OP_SETINDION				167
#define   OP_SETINDIOFF				168
#define   OP_SETTONE				169
#define   OP_MAKEWARNTONE			170
#define   OP_SETPOWERMODE			171
#define   OP_MASKSYMKEY				172
#define   OP_MASKFNTKEY				173
#define   OP_CLEARCHARBUF			174
#define   OP_SETKEYTONE				175
#define   OP_STRINGMENU				176
#define   OP_VSCROLLBAR				177
#define   OP_SHADOWEDGERECT			178
#define   OP_STRINGDIALOGBOX		179
#define   OP_STRINGPHONEMENU		180
#define   OP_BUTTON					181
#define   OP_CHECKBOX				182
#define   OP_RADIOBUTTON			183


#define OP_GETKEY				100

#define MAX_TOKEN				150			/* Tokens in general	*/
#define MAX_COMPILETOKEN		150			/* Tokens with syntax	*/

/* Error codes					*/


#define ERR_OK				     0
#define ERR_EXPECED_VAR			 1
#define ERR_EXPECED_AS			 2
#define ERR_EXPECED_TYPE		 3
#define ERR_VAR_NOT_DEF			 4
#define ERR_EXPECED_EQUAL		 5
#define ERR_EXPECED_NEWLINE		 6
#define ERR_EXPECED_TO			 7
#define ERR_VAR_NOT_MATCH		 8
#define ERR_SYNTAX				 9
#define ERR_END                  10
#define ERR_NOT_A_VAR			 11
#define ERR_OP_NOT_A_LABEL       12
#define ERR_LABEL_NOT_DEFINED    13
#define ERR_NEWVAR_IN_PASS2		 14
#define ERR_EXPECTED_BRACKET	 15
#define ERR_OVERFLOW_ON_EXPR     16
#define ERR_OUT_OF_MEMORY	     17


#define PUSH_STACK8(x)   *(--gStackPtr) = x
#define PUSH_STACK16(x)  *(--((U16*)gStackPtr)) = x
#define PUSH_STACK32(x)  *(--((U32*)gStackPtr)) = x
#define PUSH_STACKF(x)   *(--((float*)gStackPtr)) = x
#define POP_STACK8       *((U8 *)gStackPtr)++
#define POP_STACK16      *((U16*)gStackPtr)++
#define POP_STACK32      *((U32*)gStackPtr)++
#define POP_STACKF       *((float*)gStackPtr)++


/* Load a constant to the stack */

#define TYP_NULL			0xFF		/* Also: String after dispose			*/
#define TYP_8				0x00
#define TYP_16				0x01
#define TYP_32				0x02	
#define TYP_F				0x03
#define TYP_STR_C		    0x04		/* Constant string						*/ 
#define TYP_STR_D			0x05		/* Disposable string					*/
										/*  temp string, requires unwinding		*/ 
#define TYP_STR_V			0x06		/* String pointer from variable			*/
#define TYP_STR_IV			0x07		/* Pointer to variable                  */
#define TYP_LABEL			0x10		/* not used in aritmethics				*/ 
#define ADR_INDEX			0x10
#define ADR_C				0x00
#define ADR_V				0x04
#define ADR_BP				0x08
#define ADR_UNUSED			0x0C



#define COND_NULL			0x00
#define COND_Z				0x01
#define COND_NZ				0x02
#define COND_LT				0x03
#define COND_LE				0x04
#define COND_GT				0x05
#define COND_GE				0x06
#define RELATIVE			0x08


#define CMD_LOAD			0x00		/* Load a value to the stack        */
#define CMD_STORE			0x20		/* Save a value to memory       	*/
#define CMD_DUP				0x20		/* Duplicate value on the stack     */
#define CMD_EXT				0xF0		/* Extend the last stack			*/

#define CMD_PUSH_BP			0x3D
#define CMD_POP_BP			0x2D
#define CMD_LD_BP_SP		0x3C
#define CMD_LD_SP_BP		0x2C
#define CMD_PUSH_VP			0x3E
#define CMD_POP_VP			0x2E
#define CMD_PUSH_F			0x3F
#define CMD_POP_F			0x2F

#define CMD_ADDSP			0x63	/* Drop, incr. Stack Ptr			*/	/* one U8 param */

/* Arithmetic operations */
#define CMD_ADD             0x40
#define CMD_SUB				0x44
#define CMD_MULT			0x48
#define CMD_DIV				0x4C
#define CMD_CMP				0x50
#define CMD_NOT				0x5C
#define CMD_AND				0x54
#define CMD_OR				0x58
/* Jump and call */

#define	CMD_CALL			0x78
#define CMD_CALL_REL		0x79
#define CMD_RTS				0x7A
#define CMD_JUMP			0x70


/* Load constant, the constant is stored in the next few bytes */
/* Stores the constant on the stack							  */
/* Strings are handled like U32 depending on system			  */

#define CMD_LOAD_C8			CMD_LOAD  + ADR_C + TYP_8
#define CMD_LOAD_C16		CMD_LOAD  + ADR_C + TYP_16		
#define CMD_LOAD_C32		CMD_LOAD  + ADR_C + TYP_32
#define CMD_LOAD_CF			CMD_LOAD  + ADR_C + TYP_F
#define CMD_LOAD_V8			CMD_LOAD  + ADR_V + TYP_8
#define CMD_LOAD_V16		CMD_LOAD  + ADR_V + TYP_16		
#define CMD_LOAD_V32		CMD_LOAD  + ADR_V + TYP_32
#define CMD_LOAD_VF			CMD_LOAD  + ADR_V + TYP_F
#define CMD_STORE_V8		CMD_STORE  + ADR_V + TYP_8
#define CMD_STORE_V16		CMD_STORE  + ADR_V + TYP_16		
#define CMD_STORE_V32		CMD_STORE  + ADR_V + TYP_32
#define CMD_STORE_VF		CMD_STORE  + ADR_V + TYP_F

#define CMD_EXT_8_16		CMD_EXT	   + (TYP_8 << 2) + TYP_16
#define CMD_EXT_8_32		CMD_EXT	   + (TYP_8 << 2) + TYP_32
#define CMD_EXT_8_F			CMD_EXT	   + (TYP_8 << 2) + TYP_F 
#define CMD_EXT_16_8 		CMD_EXT	   + (TYP_16 << 2) + TYP_8
#define CMD_EXT_16_32		CMD_EXT	   + (TYP_16 << 2) + TYP_32
#define CMD_EXT_16_F		CMD_EXT	   + (TYP_16 << 2) + TYP_F 
#define CMD_EXT_32_8		CMD_EXT	   + (TYP_32 << 2) + TYP_8
#define CMD_EXT_32_16		CMD_EXT	   + (TYP_32 << 2) + TYP_16
#define CMD_EXT_32_F		CMD_EXT	   + (TYP_32 << 2) + TYP_F 
#define CMD_EXT_F_8			CMD_EXT	   + (TYP_F << 2) + TYP_8
#define CMD_EXT_F_16		CMD_EXT	   + (TYP_F << 2) + TYP_16
#define CMD_EXT_F_32		CMD_EXT	   + (TYP_F << 2) + TYP_32

#define CMD_ADD_8			CMD_ADD + TYP_8
#define CMD_ADD_16			CMD_ADD + TYP_16
#define CMD_ADD_32			CMD_ADD + TYP_32
#define CMD_ADD_F			CMD_ADD + TYP_F
#define CMD_SUB_8			CMD_SUB + TYP_8
#define CMD_SUB_16			CMD_SUB + TYP_16
#define CMD_SUB_32			CMD_SUB + TYP_32
#define CMD_SUB_F			CMD_SUB + TYP_F
#define CMD_MULT_8			CMD_MULT + TYP_8
#define CMD_MULT_16			CMD_MULT + TYP_16
#define CMD_MULT_32			CMD_MULT + TYP_32
#define CMD_MULT_F			CMD_MULT + TYP_F
#define CMD_DIV_8			CMD_DIV + TYP_8
#define CMD_DIV_16			CMD_DIV + TYP_16
#define CMD_DIV_32			CMD_DIV + TYP_32
#define CMD_DIV_F			CMD_DIV + TYP_F
#define CMD_CMP_8			CMD_CMP + TYP_8
#define CMD_CMP_16			CMD_CMP + TYP_16
#define CMD_CMP_32			CMD_CMP + TYP_32
#define CMD_CMP_F			CMD_CMP + TYP_F
#define CMD_AND_8			CMD_AND + TYP_8
#define CMD_AND_16			CMD_AND + TYP_16
#define CMD_AND_32			CMD_AND + TYP_32
#define CMD_OR_8			CMD_OR + TYP_8
#define CMD_OR_16			CMD_OR + TYP_16
#define CMD_OR_32			CMD_OR + TYP_32


#define CMD_JUMP_Z			CMD_JUMP + COND_Z
#define CMD_JUMP_NZ			CMD_JUMP + COND_NZ
#define CMD_JUMP_GT			CMD_JUMP + COND_GT
#define CMD_JUMP_GE			CMD_JUMP + COND_GE
#define CMD_JUMP_LT			CMD_JUMP + COND_LT
#define CMD_JUMP_LE			CMD_JUMP + COND_LE
#define CMD_JUMP_R			CMD_JUMP + RELATIVE
#define CMD_JUMP_R_Z		CMD_JUMP + RELATIVE + COND_Z
#define CMD_JUMP_R_NZ		CMD_JUMP + RELATIVE + COND_NZ
#define CMD_JUMP_R_GT		CMD_JUMP + RELATIVE + COND_GT
#define CMD_JUMP_R_GE		CMD_JUMP + RELATIVE + COND_GE
#define CMD_JUMP_R_LT		CMD_JUMP + RELATIVE + COND_LT
#define CMD_JUMP_R_LE		CMD_JUMP + RELATIVE + COND_LE

#define CMD_LOADF			0xB0				
#define CMD_LOADF_Z			CMD_LOADF +	COND_Z				
#define CMD_LOADF_NZ		CMD_LOADF +	COND_NZ				
#define CMD_LOADF_LT		CMD_LOADF +	COND_LT				
#define CMD_LOADF_LE		CMD_LOADF +	COND_LE				
#define CMD_LOADF_GT		CMD_LOADF +	COND_GT				
#define CMD_LOADF_GE		CMD_LOADF +	COND_GE				



#define CMD_INC				0xB8
#define CMD_DEC				0xB9

/* Basic Operations */
#define CMD_SOURCELINE		0x90
#define CMD_LOAD_STR_C		0x91				/* Push a constant string */
#define CMD_LOAD_STR_V		0x92
#define CMD_STORE_STR_V		0x93
#define CMD_STRINGADD		0x94
#define CMD_STRINGCMP		0x95
#define CMD_STORE_STR_VI	0x96
#define CMD_LOAD_STR_VI		0x97

#define CMD_GETKEY			0xA0
#define CMD_LEN				0xA1
#define CMD_ASC				0xA2
#define CMD_CHR				0xA3
#define CMD_VAL				0xA4
#define CMD_STR				0xA5
#define CMD_MID				0xA6
#define CMD_LEFT			0xA7
#define CMD_RIGHT			0xA8






#define CMD_INPUTI			0x99
#define INPUT_8				CMD_INPUTI + TYP_8
#define INPUT_16			CMD_INPUTI + TYP_16
#define INPUT_32			CMD_INPUTI + TYP_32
#define INPUT_F				CMD_INPUTI + TYP_F
#define CMD_PRINTI			0x84
#define CMD_PRINT_8			CMD_PRINTI + TYP_8
#define CMD_PRINT_16		CMD_PRINTI + TYP_16
#define CMD_PRINT_32		CMD_PRINTI + TYP_32
#define CMD_PRINT_F			CMD_PRINTI + TYP_F
#define CMD_PRINT_STR		0x88 + 3
#define CMD_INPUT_STR		0x88 + 4
#define CMD_PRINT_TAB		0x88 + 1
#define CMD_PRINT_NL        0x88 + 2
#define CMD_PLOT			0xE0
#define CMD_LINE			0xE1
#define CMD_CLS				0xE2
#define CMD_GOTOXY			0xE3
#define CMD_SIN				0xE4
#define CMD_COS				0xE5
#define CMD_TAN				0xE6
#define CMD_DATE_S			0xE7
#define CMD_TIME_S			0xE8
#define CMD_RAND_F			0xE9
#define CMD_RANDOMIZE		0xEA

#define CMD_API				0xEF


#define CMD_HALT			0xFF


#define MAXAPICALLS				0x22

#define CMD_API_FILLBLOCK			0x00
#define CMD_API_FILLSCREEN			0x01
#define CMD_API_INVERSEBLOCK		0x02
#define CMD_API_VLINE				0x03
#define CMD_API_HLINE				0x04
#define CMD_API_RECTANGLE			0x05
#define CMD_API_CURSORSTATUS		0x06
#define CMD_API_CURSORSHAPE			0x07
#define CMD_API_FONT				0x08
#define CMD_API_SCROLLUP			0x09
#define CMD_API_SCROLLDN			0x0A
#define CMD_API_SCROLLLEFT			0x0B
#define CMD_API_SCROLLRIGHT			0x0C
#define CMD_API_RESTORELCD			0x0D
#define CMD_API_BACKUPLCD			0x0E
#define CMD_API_RESTOREHELPLCD		0x0F
#define CMD_API_BACKUPHELPLCD		0x10
#define CMD_API_SETINDION			0x11
#define CMD_API_SETINDIOFF			0x12
#define CMD_API_SETTONE				0x13
#define CMD_API_MAKEWARNTONE		0x14
#define CMD_API_SETPOWERMODE		0x15
#define CMD_API_MASKSYMKEY			0x11
#define CMD_API_MASKFNTKEY			0x17
#define CMD_API_CLEARCHARBUF		0x18
#define CMD_API_SETKEYTONE			0x19
#define CMD_API_STRINGMENU			0x1A
#define CMD_API_VSCROLLBAR			0x1B
#define CMD_API_SHADOWEDGERECT		0x1C
#define CMD_API_STRINGDIALOGBOX		0x1D
#define CMD_API_STRINGPHONEMENU		0x1E
#define CMD_API_BUTTON				0x1F
#define CMD_API_CHECKBOX			0x20
#define CMD_API_RADIOBUTTON			0x21

#define CMD_API_MAX				    0x21

#define U8 unsigned char

typedef struct tagCompileToken
{
	U8 token;
	U8 op1;
	U8 op2;
	U8 returntype;
	U8 *Syntax;
} CompileToken;

CompileToken gCompileToken2[53] =
{
	{OP_LINE,	CMD_LINE,0,			TYP_NULL, "biiii" },
	{OP_GOTOXY,  CMD_GOTOXY,0,		TYP_NULL,"bb" },
	{OP_DOT,		CMD_PLOT,0,			TYP_NULL,"bbb" },

	{OP_FILLBLOCK,		CMD_API, CMD_API_FILLBLOCK,			TYP_NULL, "biiii" },
	{OP_FILLSCREEN,		CMD_API, CMD_API_FILLSCREEN,		TYP_NULL, "b" },
	{OP_INVERSEBLOCK,		CMD_API, CMD_API_INVERSEBLOCK,		TYP_NULL, "iiii" },
	{OP_VLINE,			CMD_API, CMD_API_VLINE,				TYP_NULL, "biiib" },
	{OP_HLINE,			CMD_API, CMD_API_HLINE,				TYP_NULL, "biiib" },
	{OP_RECTANGLE,		CMD_API, CMD_API_RECTANGLE,			TYP_NULL, "biiiib" },
	{OP_CURSORSTATUS,		CMD_API, CMD_API_CURSORSTATUS,		TYP_NULL, "b" },
	{OP_CURSORSHAPE,		CMD_API, CMD_API_CURSORSHAPE,		TYP_NULL, "ii" },
	{OP_FONT,				CMD_API, CMD_API_FONT,				TYP_NULL, "b" },
	{OP_SCROLLUP,			CMD_API, CMD_API_SCROLLUP,			TYP_NULL, "iiiiib" },
	{OP_SCROLLDN,			CMD_API, CMD_API_SCROLLDN,			TYP_NULL, "iiiiib" },
	{OP_SCROLLLEFT,		CMD_API, CMD_API_SCROLLLEFT,		TYP_NULL, "iiiiib" },
	{OP_SCROLLRIGHT,		CMD_API, CMD_API_SCROLLRIGHT,		TYP_NULL, "iiiiib" },
	{OP_RESTORELCD,		CMD_API, CMD_API_RESTORELCD,		TYP_NULL, "iiii" },
	{OP_BACKUPLCD,		CMD_API, CMD_API_BACKUPLCD,			TYP_NULL, "iiii" },
	{OP_RESTOREHELPLCD,	CMD_API, CMD_API_RESTOREHELPLCD,	TYP_NULL, "" },
	{OP_BACKUPHELPLCD,	CMD_API, CMD_API_BACKUPHELPLCD,		TYP_NULL, "" },
	{OP_SETINDION,		CMD_API, CMD_API_SETINDION,			TYP_NULL, "b" },
	{OP_SETINDIOFF,		CMD_API, CMD_API_SETINDIOFF,		TYP_NULL, "b" },
	{OP_SETTONE,			CMD_API, CMD_API_SETTONE,			TYP_NULL, "i" },
	{OP_MAKEWARNTONE,		CMD_API, CMD_API_MAKEWARNTONE,		TYP_NULL, "" },
	{OP_SETPOWERMODE,		CMD_API, CMD_API_SETPOWERMODE,		TYP_NULL, "b" },
	{OP_MASKSYMKEY,		CMD_API, CMD_API_MASKSYMKEY,		TYP_NULL, "b" },
	{OP_MASKFNTKEY,		CMD_API, CMD_API_MASKFNTKEY,		TYP_NULL, "b" },
	{OP_CLEARCHARBUF,		CMD_API, CMD_API_CLEARCHARBUF,		TYP_NULL, "" },
	{OP_SETKEYTONE,		CMD_API, CMD_API_SETKEYTONE,		TYP_NULL, "b" },
	{OP_VSCROLLBAR,		CMD_API, CMD_API_VSCROLLBAR,		TYP_NULL, "iiiiii" },
	{OP_SHADOWEDGERECT,	CMD_API, CMD_API_SHADOWEDGERECT,	TYP_NULL, "iiii" },
	{OP_BUTTON,			CMD_API, CMD_API_BUTTON,			TYP_NULL, "siii" },



	{0,0,0,TYP_NULL, 0},
	{OP_LEN,		CMD_LEN,0,			TYP_16, "s" },
	{OP_ASC,		CMD_ASC,0,			TYP_16, "s" },
	{OP_CHR,		CMD_CHR,0,			TYP_STR_C,"i" },
	{OP_VAL,		CMD_VAL,0,			TYP_F,"s" },
	{OP_STR,		CMD_STR,0,			TYP_STR_C,"f" },
	{OP_MID,		CMD_MID,0,			TYP_STR_C,"sii" },
	{OP_LEFT,	CMD_LEFT,0,			TYP_STR_C,"si" },
	{OP_RIGHT,	CMD_RIGHT,0,		TYP_STR_C,"si" },
	{OP_RAND,	CMD_RAND_F,0,			TYP_16,"i" },
	{OP_SIN,     CMD_SIN,0,          TYP_F,"f" },
	{OP_COS,     CMD_SIN,0,          TYP_F,"f" },
	{OP_TAN,     CMD_SIN,0,          TYP_F,"f" },
	{OP_DATE,	CMD_DATE_S,0,		TYP_STR_C,"" },
	{OP_TIME,	CMD_TIME_S,0,		TYP_STR_C,"" },



	{OP_STRINGMENU,		CMD_API, CMD_API_STRINGMENU,		TYP_16,		"si" },
	{OP_STRINGDIALOGBOX,	CMD_API, CMD_API_STRINGDIALOGBOX,	TYP_16,		"si" },
	{OP_STRINGPHONEMENU,	CMD_API, CMD_API_STRINGPHONEMENU,	TYP_16,		"si" },
	{OP_CHECKBOX,			CMD_API, CMD_API_CHECKBOX,			TYP_STR_D,  "ssii" },
	{OP_RADIOBUTTON,		CMD_API, CMD_API_RADIOBUTTON,		TYP_16,		"siii" },

	{0,0,0,TYP_NULL, 0}
};

