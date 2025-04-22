
#ifndef __DATA_H__
#define __DATA_H__

#ifdef _WIN32
#define UPTR		U32
#define USTRLEN		S16
#define SIZE_T		S32
#else
#define UPTR		U16
#define USTRLEN		S16
#define SIZE_T		S16
#endif

#define   STD_BANK	3

#include "icstdioq.h"

extern ICTerminal gTerm;


#define LABEL_DEF			/* Definition of a label */
#define LABEL_UNRESOLVED	/* Unresolved label, the address point to the address, */
							/* where the resulting address must be inserted on the */
                            /* resolve step                                        */


typedef void (VMCOMMAND)();	/* All processor command are on a table, we simply jump */
                            /* to it using this type                                */

#define STACKSIZE    1024	/* Size of stack for stack machine						*/
#define CODESIZE     1024	/* Maximum size of code									*/
#define SOURCESIZE   4096
#ifdef _WIN32
#define MEMORYSIZE   100*1024 /* Size of memory (compile and run)						*/
#else
#define MEMORYSIZE   14*1024 /* Size of memory (compile and run)						*/
#endif
#define VARIABLESIZE  200	 /* Number of variables									*/
#define MAX_HIER	   50
#define PRINTBUFSIZE  256


typedef struct tagStrVar {
	U16 m_Size;					/* Length of string							*/
	U8  m_Flags;				/* Typ of string (disposable, constant etc. */
	U8  *m_String;				/* Pointer to string						*/
	
} StrVar;

typedef struct tagICSettings
{
	S16		Size_Sourcecode,
			Size_Code,
			Size_Stack,
			Size_Variables,
			Size_Heap;
			
} ICSettings;
	

/* Variables are stored in a variable space											*/
/* During run time, we do not need any more the name and the type					*/
/* Except we want to imlement a debugger											*/

typedef struct tagVariable {
	U16 m_ElemSize;
	U16 m_Size;
	U8 cDim;
	U16 m_Dim[3];
	U8 m_Typ;				/* While parsing: Variable typ							*/
	UPTR m_Offset;
	char *m_Name;			/* While parsing: Variable name							*/
	void *m_Next;			/* Actually of Type variable						    */
	void *m_TypeInfo;		/* Actually of Type variable						    */
} Variable;


/* Labels are used during compile they will be stored in our stack space.
   Strings are stored in the memory pool
*/
typedef struct tagCompilerLabel {
    U8  *name;
	U8  typ;
	U16 address;
} CompilerLabel;

/* HierInfo is used for hierrarchial information such as for/next while/wend do/loop */
/* Since it is like a stack, we use the stack space for it						     */

#define HIER_FOR	 1
#define HIER_WHILE	 2
#define HIER_DO		 3
#define HIER_IF      4
#define HIER_RESOLVE 0x80


typedef struct 
{
	U16    hierTyp;						/* Type of structure						*/
	U16    hierVar;						/* For needs a variable                     */
	U16    *hierExit;					/* Address of exit							*/
	U8     *label1,*label2;
	U16    hierAddress;					/* Address, where structure starts			*/
} HierInfo;

typedef struct 
{
    U8     m_Name[12];
    U16    m_SizeCode;
	U16    m_SizeSymboltable;
	U16    m_SizeVariables;
	U16    m_StackSizeNeeded;
	U16    m_HeapSizeNeeded;
} CodeHeader;


#define HIERSIZE (STACKSIZE/sizeof(HierInfo))

	




/* Tokens for parsing the BASIC language*/
typedef struct tagBasicToken
{
	U8 *name;
	U8 token;
	U8 returntype;
	U8 op1;
	U8 op2;
	U8 *syntax;
	U8 *help;
} BasicToken;

/* This table is used to compile standard functions, such as dot or line which
   have a fixed syntax (e.g. no variants for other types etc.



  Syntax consists of one char per parameter:
     b = byte
	 i = integer
	 l = long
	 f = float
	 s = string
*/

typedef struct tagCompileToken
{
	U8 token;
	U8 op1;
	U8 op2;
	U8 returntype;
	U8 *Syntax;
} CompileToken;

typedef struct tagToken {
	U8		m_token;
	U8		*m_input;
	U8		*m_word;
	Variable m_Variable;
};

extern U8 gFileName[];
extern U8 gFlagZero;						/* Processor flags                        */
extern U8 gFlagLess;
extern U8 gOrgRamBank;
/*
extern CompileToken   *gFuncSyntax;
extern CompileToken	   gCompileToken[];
*/
extern BasicToken		*gLastTokenPtr;

extern U8 gPrintBuf[PRINTBUFSIZE];

extern U8 *gStack;				/* Stack space							  */
extern U8 *gStackPtr;

extern U8 *gCodeBase;
extern U8 *gCode;
extern U8 *gCodePtr;
extern U8 *gLastPC;
extern U8 *gRegisterVP;						/* The Variable Pointer register */


/* During compile, we store variables in records, containing the type 
   (which may be LABEL also), the name and everything. During runtime
   we only have a block of memory, where all data from variables is stored.
    
   In order to debug, we may need a variable table containing name/size and
   offset
*/
extern U8 *gCompareOp;
extern Variable *gVariables;	/* Variable space                        */
extern U16 gLastVar;
extern U16 gVariablesSize;					/* Size of variable table */

extern U8 gCompilerInsertDebug;
extern U8 gCompilerShowListing;
extern U8 gScreenLine;
extern U8 gPass;
extern U8 *gSource;
extern U8 *gSourcePtr;
extern U8 *gLastSourcePtr;
extern U8 *gOldSourcePtr;
extern U8 gTokenBuf[50];
extern U16 gLine;
extern U8 gLastToken;
/* buffer for variable operation, temp token name and temp string */
extern U8 gTempBuf[256];
extern U8 gTempEnd;

/* literals during parse */
extern U32 gLitLong;
extern U16 gLitInt;
extern U8  gLitShort;
extern float gLitFloat;
extern double gLitDouble;

extern U8 gFileLoaded;
extern U8 gFileDirty;

extern U8  gExprType;
extern U8  gExprCastType;
extern U8  gStringBuf[256];
extern U8  gErrorCode;
extern U8 gMemory[];
extern U8 *gMemoryPtr;
extern U8 *gBASICMemoryStart;
extern U8 *gBASICMemoryEnd;				
extern U8 *gBASICMemoryPtr;					
extern U8 *gHeap;
extern U16 gHeapCount;
extern U8 *gHeapTop;
extern U8 *gHeapLast;
extern U16 gMaxVariable;
extern U16 gMaxHier;
extern U8 gDebug;

extern BasicToken gBasToken[];
extern VMCOMMAND* gCmdJumpTable[256];
extern VMCOMMAND* gAPICmdJumpTable[];
extern ICSettings gSettings;
extern char *Errors[];
extern char *__nl;
extern char *__errUnexp;
extern U8 _data_Begin;
extern U8 _data_End;

#define BTN(x) { strcpy(gTempBuf,x); Button(gTempBuf,80,120,1); }
#define BTN2(x) { strcpy(gTempBuf,x); Button(gTempBuf,80,110,1); GetKey();}
#define TXT(x,y,t) { strcpy(gPrintBuf,t); WriteText(1,t,x,y); }

#endif