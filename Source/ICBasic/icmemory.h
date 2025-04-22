#ifndef _ICMEMORY_
#define _ICMEMORY_

/* Describes a memory block for string handling.
   Memory pointers contain a back pointer, so we can move the memory block
   around.
  
  
*/
typedef struct tagICMemory
{
	U16 magic;
	U16 Len;			/* Relativ pointer to next block		*/
	U8  Used;			/* 1 if this block is free				*/
	U8 **Handle;			/* Pointer to where this block is used  */
} ICMemory;

char	   *ic_strdup(char *);
void	   *ic_malloc(U16 size);
void		ic_initMemory();
void		ic_InitSettings();
void		ic_InitalAllocMemory();
U8			BASIC_DisposeMemory(U8 *memory);
U8         *BASIC_AllocateString(USTRLEN size, U8 **Handle);
void		BASIC_SetHandle(U8 *memory, U8 **Handle);
void		ic_initMemory();

#endif