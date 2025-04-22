

#ifdef _WIN32
#include "stdio.h"
#endif
#include "ic35sdk.h"
#include "data.h"
#include "icmemory.h"
#include "compiler.h"
#include "icmemory.h"
#ifdef _WIN32
#include <crtdbg.h>
#endif

#define USE_TCP_MEM

#define MEM_USED   1
#define MEM_UNUSED 2

/* Create Initial memory settings */

void ic_InitSettings()
{

#ifdef _WIN32
	gSettings.Size_Sourcecode = 20*1024;
	gSettings.Size_Heap       = 20*1024;
	gSettings.Size_Code       = 20*1024;
	gSettings.Size_Stack      = 20*1024;
	gSettings.Size_Variables  = 20*1024;

#else
	U16  m1,m2;
// we have 14 KB
	gSettings.Size_Sourcecode = 8*1024;
	gSettings.Size_Heap       = 3*1024;
	gSettings.Size_Stack      = 1*1024;
	gSettings.Size_Variables  = 2*1024;

// this goes to the TCP/IP memory
	gSettings.Size_Code       = 4*1024;

/*	
	m1 = 0xC000;
	m2 = gMemory;
	m1 = m2 - m1;
	m2 = gSettings.Size_Sourcecode +
		 gSettings.Size_Code +
		 gSettings.Size_Stack +
		 gSettings.Size_Variables;

	strcpy(gTempBuf,"Max Heap ");
	ltoa(m2,gPrintBuf,10);
	strcat(gTempBuf,gPrintBuf);
	StringDialogBox(gTempBuf,0);
	gSettings.Size_Heap = m1 - m2;
*/
#endif

}



/* Do the memory allocation */

void ic_InitalAllocMemory()
{
	U8 *p = gMemory;

#ifdef _WIN32
	gSource		= p;				p+= gSettings.Size_Sourcecode;
	gCode		= p;				p+= gSettings.Size_Code;
	gVariables	= (Variable*)p;		p+= gSettings.Size_Variables;
	gHeap       = p;				p+= gSettings.Size_Heap;
	gStack      = p;
#else
	gSource		= p;				p+= gSettings.Size_Sourcecode;
	gVariables	= (Variable*)p;		p+= gSettings.Size_Variables;
	gHeap       = p;				p+= gSettings.Size_Heap;
	gStack      = p;

	gCode		= 0xD600;
#endif
	gHeapTop	= gHeap;


	if (gSettings.Size_Code == 0)		BTN2("Codesize = 0");
	if (gSettings.Size_Sourcecode == 0) BTN2("Sourcecodesize = 0");
	if (gSettings.Size_Variables == 0)	BTN2("Varsize = NULL");
	if (gSettings.Size_Heap == 0)		BTN2("Heap = NULL");

	/* Edit environment and run time environment use the same
	   memory block, but different allocation routines         */
	

	gHeapLast	 = NULL;
	gMaxVariable = (U16) gSettings.Size_Variables / sizeof(Variable);
}

/* Init Memory for BASIC runtime */

void ic_InitHeap()
{
	gHeapTop  = gHeap;
	gHeapLast = NULL;
	memset(gVariables,gSettings.Size_Variables,0);	/* Set Variable aera to 0 */
}


/* Init memory for compile/edit environment */

void ic_initMemory()
{
	gMemoryPtr = gHeap;
	memset(gMemoryPtr,0,gSettings.Size_Heap); 
}

void *_findFreeMem(U16 size)
{
	U8 *p = gMemory;
	U8 *p1,c;
	U16 len;
	void *r = NULL;

	while (TRUE)
	{
		c = *p++;
		if (c == 0) return NULL;
		len = *((U16*)p)++;
		if (c == MEM_UNUSED) {
			
			/*
			mp = (U16*)p;
			len = *mp++;
			p = (U8*)mp;
			*/
			/* split memory  */
			if (len > size) {
				*p++ = MEM_USED;
				*(((U16*)p)++) = size;
				r = p;
				p1 = p+size;
				*p1++ = MEM_UNUSED;
				*((U16*)p)++ = 1234;
				break;
			}
		}
		p+=len;


	}
	return r;
}
/* We use malloc only during compile. The run time uses Basic_xxx routines
   to get memory
*/

void *ic_malloc(U16 size)
{
	void *r;

/*	r = _findFreeMem(size); */
	r = NULL;
	if (r == NULL)
	{
		*gMemoryPtr++ = MEM_USED;
		*((U16*)gMemoryPtr)++ = size;
		/*
		mp = (U16*)gMemoryPtr;
		*mp++=size;
		gMemoryPtr = (U8*)mp;
		*/
		
		r = (void*)gMemoryPtr;
		gMemoryPtr += size;
		gHeapTop = gMemoryPtr;
	}
	return r;
}

char *ic_strdup(char *s)
{
	char *dest;
	U16  len;

	len = strlen(s)+1;
	dest = (char*)ic_malloc(len);
	strcpy(dest,s);
	return dest;
}

/*
 ----------------------------------------------------------------------------
 String operations for BASIC
 ----------------------------------------------------------------------------
*/

void BASIC_CollectGarbage()
{
	ICMemory *s, *d,*last,*first;

	U8 **h;
	U8 *p,*next;
	U16 size,ssize;
	U16 cnt;


	s = (ICMemory*)gHeap;
	d = (ICMemory*)gHeap;
	ssize = sizeof(ICMemory);
	first = s;

	cnt = 0;
	while ((U8*)s < gHeapTop)
	{
		if (s->Used) {
			if (s != d) {
				size = s->Len + sizeof(ICMemory);
				next = (U8*)s;
				next += sizeof(ICMemory)+s->Len;

				memcpy(d,s,size);
				
				h = d->Handle;
				*h = (char*)(d+1);
//				*h = d+sizeof(ICMemory);
				last = d;
				p = (U8*)d;
				p += d->Len + sizeof(ICMemory);
				d= (ICMemory*)p;
				s = (ICMemory*)next;
				cnt++;

				
			} else {
				p = (U8*)s;
				p += sizeof(ICMemory)+s->Len;
				s = (ICMemory*)p;
			}
		}
		else
		{
			p = (U8*)s;
			p += sizeof(ICMemory)+s->Len;
			s = (ICMemory*)p;
		}

		
	}
	gHeapCount = cnt;
	gHeapTop = (U8*)d;
	memset(gHeapTop,(gHeap+gSettings.Size_Heap) - gHeapTop,0);
}

void BASIC_SetHandle(U8 *memory, U8 **Handle)
{
	ICMemory *mem;

	memory -= sizeof(ICMemory);
	mem = (ICMemory*)memory;
	mem->Handle = Handle;
}


U8 *BASIC_AllocateString(USTRLEN size, U8 **Handle)
{
	U8 *m;
	char **h;
	SIZE_T sizeRemaining;
	ICMemory *newMem;
	sizeRemaining = gStack-gHeapTop;

	newMem = NULL;
	if (size + sizeof(ICMemory) > sizeRemaining || sizeRemaining < 0)
	{
		BASIC_CollectGarbage();
		sizeRemaining = gStack-gHeapTop;
	}
	if (size + sizeof(ICMemory) > sizeRemaining || sizeRemaining < 0) 
	{
		gErrorCode = ERR_OUT_OF_MEMORY;
		return NULL;
	}

	if (gHeapLast != NULL)
	{
		newMem = (ICMemory*)gHeapLast;
		
		if (newMem->Used == 0)   // the last memory block is free, reuse it.
		{
			newMem->magic = 0x1234;
			newMem->Len = size;
			newMem->Used = 1;
			newMem->Handle = Handle;
			gHeapTop = (U8*)newMem + size + sizeof(ICMemory);
		}
		else
		
			newMem = NULL;
	}
    if (newMem == NULL)

	{
		newMem = (ICMemory*)gHeapTop;
		newMem->magic = 0x1234;
		newMem->Len = size;
		newMem->Used = 1;
		newMem->Handle = Handle;
		gHeapLast = gHeapTop;
		gHeapTop += size + sizeof(ICMemory);
	}

	if (newMem == NULL)
		return NULL;			/* Report Out of memory */
	m = (U8*)newMem;
	h = (char**)Handle;
	m+=sizeof(ICMemory);
	*m = 0;
	gHeapCount++;
	if (m > gStackPtr-100)
		return 0;

	return m;
}

U8 BASIC_DisposeMemory(U8 *memory)
{
	ICMemory *mem;
	char **h;


	memory -= sizeof(ICMemory);
	mem = (ICMemory*)memory;

	h = (char**)mem->Handle;
	mem->Handle = 0;
	mem->Used = 0;
	gHeapCount++;
	return 0;
}