#include "data.h"


typedef U8 (ONSCROLL)(U16 diff,void *editor);		
typedef U8 (ONEDITOREVENT)(void *editor);		

#define EDITOR_NULL				0
#define EDITOR_EXIT				100

typedef struct tagICEditor
{
	U8   x,y,w,h;				/* Editor coordinates					*/
	U16  line;					/* Current line							*/
	U8  *pLine;					/* Pointer to line						*/
	U8   *buffer;				/* Pointer to buffer					*/
	U16  bufferSize;			/* Length of buffer						*/
	U8   ScrollX;				/* Charactes the editor is scrolled		*/
    U16  TopLine;				/* Line on the top						*/
	U8   *pTopLine;				/* Pointer to top line					*/
	U8   LineModified;
	U8   Modified;
	U8   InsertMode;

	U8  *LineBuffer;			
	U8   LineBufferSize;
	U8  *pPosInLine;			/* For use in Event handler				*/
	U8   EventCode;				
	U8   EventReturnCode;	
	U16  EventEditorReturn;		/* Return code for editor, may be set on event */


	ONSCROLL *OnScroll;			/* Call this, wenn scrolling happens    */
	ONEDITOREVENT *OnMenu;		/* Execute menu							*/
	ONEDITOREVENT *OnChar;		/* Called if a character is typed		*/
} ICEditor;



void ICEditFile(ICEditor *editor, char *buffer, U16 length, U16 line);

U8 SelectFile(U8 *Directory, U8 *FileName,U8 *Filter, U8 *Title);
void InitEditor(ICEditor *editor);
