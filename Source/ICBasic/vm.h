/* commands */

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
#define CMD_POKE			0xA9
#define CMD_PEEK			0xAA
#define CMD_IN				0xAB
#define CMD_OUT				0xAC


// ABS: 0xC0 .. 0xC3
#define CMD_ABS				0xC0
#define CMD_ABS_8			CMD_ABS + TYP_8
#define CMD_ABS_16			CMD_ABS + TYP_16
#define CMD_ABS_32			CMD_ABS + TYP_32
#define CMD_ABS_F			CMD_ABS + TYP_F


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
#define CMD_BREAKPOINT		0xEB
#define CMD_SCANKEY			0xEC

#define CMD_API				0xEF


#define CMD_HALT			0xFF

typedef struct  tagICString
{
	USTRLEN	 Len;				/* Must be the first member */
	U8		 Typ;
	U8       *pString;
//	Variable *pVariable;
} ICString;


void InitVM();
void ExecuteVM(U8 *adr);
void ExecuteVMDebug(U8 *adr);
void PopString(ICString **str);
void PushString2(ICString *str,U8 typ);
void PushString(ICString *str);
ICString *CreateStackString();

#define ERROR {gLastPC = gCodePtr; gCodePtr = NULL; return; }



