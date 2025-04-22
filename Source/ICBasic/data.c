
#include <ic35sdk.h>
#include "icstdioq.h"
#include "data.h"
#include "compiler.h"



ICTerminal gTerm = {0};

U8       _data_Begin = 0;

/* --------------------------------------------------------------- */
/* General data */
/* --------------------------------------------------------------- */
U8				gTempBuf[256]					= {0};
U8				gTempEnd						= 0;
U8				gStringBuf[256]					= {0};
U8				gOrgRamBank						= 0;
U8				gFileName[64]					= {0};
U16				wFreq = 0;
S32				zg_value = 0;
U8			    gFileLoaded						= 0;
U8				gFileDirty						= 0;

/* --------------------------------------------------------------- */
/* Data for compiler */
/* --------------------------------------------------------------- */

/* BasicToken		gBasToken[MAX_TOKEN]			= {0};   
 CompileToken	gCompileToken[MAX_COMPILETOKEN] = {0}; 
CompileToken   *gFuncSyntax						= {0};		 */
BasicToken		*gLastTokenPtr					= 0;							/* Pointer to last scanned token */
char			*Errors[MAX_ERROR]				= {0};

char			*__nl							= 0;
char			*__errUnexp						= 0;

U8			   *gCompareOp						= 0;
U8				gCompilerInsertDebug			= 0;
U8				gCompilerShowListing			= 0;
U8				gDebug							= 0;
U8				gPass							= 0;
U16				gVariablesSize					= 0;			/* Size of variable table */
U16				gLastVar						= 0;
U8				gTokenBuf[50]					= {0};			/* Temp memory for storing a token */
U8				*gSourcePtr						= 0;
U8				*gLastSourcePtr					= 0;
U8				*gOldSourcePtr					= 0;
U16				gLine                           = 0;
U8				gLastToken                      = 0;

/* literals during parse */

U32				gLitLong						= 0;
U16				gLitInt							= 0;
U8				gLitShort						= 0;
float			gLitFloat						= 0;
double			gLitDouble						= 0;

U8				gExprType						= 0;
U8				gExprCastType					= 0;
U8				gErrorCode						= 0;


/* --------------------------------------------------------------- */
/* Data for stack machine */
/* --------------------------------------------------------------- */

U8 *gStackPtr = 0;
U8 gFlagZero						= {0};
U8 gFlagLess						= {0};
U8 *gCodePtr = 0;
U8 *gLastPC = 0;
U8 *gCodeBase = 0;
U8 *gRegisterVP = 0;			
VMCOMMAND *gCmdJumpTable[256]		= {0};
VMCOMMAND *gAPICmdJumpTable[45]		= {0};

/* --------------------------------------------------------------- */
/* Data for terminal */
/* --------------------------------------------------------------- */

U8 gScreenLine = 0;

U8 gPrintBuf[PRINTBUFSIZE]			= {0};


/* --------------------------------------------------------------- */
/* Unused ?*/
/* --------------------------------------------------------------- */


/* --------------------------------------------------------------- */
/* Data for memory allocation and heap */
/* --------------------------------------------------------------- */

U8 *gMemoryPtr						= 0;
/* 
U8 *gBASICMemoryStart				= 0;
U8 *gBASICMemoryEnd					= 0;
U8 *gBASICMemoryPtr					= 0;
*/




/* gMemory will hold all memory, including source, stack, 
   variables etc. */
ICSettings  gSettings						= {0};
U8			*gStack							= 0;
U8			*gSource						= 0;
Variable    *gVariables						= 0;
U8			*gHeap							= 0;
U16			gHeapCount						= 0;
U8			*gHeapTop						= 0;
U8			*gHeapLast						= 0;
U8			*gCode							= 0;

U16         gMaxVariable					= 0;

U8			gMemory[MEMORYSIZE+1]			= {0};


U8       _data_End  = 0;

