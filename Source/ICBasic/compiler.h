#define TYP_HANDLED 0xff
#define TYP_VOID    0xfe


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
	#define OP_POKE					28
	#define OP_PEEK					29
     

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
	#define OP_ABS					48



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
	#define OP_IN					76
	#define OP_OUT					77

	#define OP_SIN					90
	#define OP_COS					91
	#define OP_TAN					92
	#define OP_DATE					93
	#define OP_TIME					94
	#define OP_BREAKPOINT			95
	#define OP_HALT					96
	#define OP_SCANKEY				97


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
	#define ERR_FUNCTION_AS_STATEMENT 18
    

#define MAX_ERROR				 30

void Compile();
void ReportError();
void InitCompiler();
void LoadBasic();
void InitParser();
void InitSyntax(CompileToken **pSyn, U8 token,U8 op1, U8 op2, U8 rettype, U8 *syntax);
void InitToken(BasicToken **t, U8 *s, U8 id);
U8 Expect(U8 token);





