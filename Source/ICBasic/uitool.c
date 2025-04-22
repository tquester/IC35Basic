
#include "ic35sdk.h"
#include "uitool.h"
#include "data.h"

#ifdef _WIN32
#include "crtdbg.h"
#endif

/* Line display in Editor.
   Start displaying a line starting at ScrollX. Ensure that only up to 239 Pixels are
   drawn. 

*/

#define FILES_PER_PAGE 9

void _addi(U16 i)
{
	U8 buf[15];

	itoa(i,buf,10);
	strcat(gTempBuf,buf);
}
_clear()
{
	*gTempBuf = 0;
}

_add(U8 *s)
{
	strcat(gTempBuf,s);
}

_txt(U8 *s)
{
	strcpy(gTempBuf,s);
}

_show()
{
		SetCursorStatus(0);
		FillBlock(1,0,148,159,12);
		WriteText(0,gTempBuf,0,150);
//		GetKey();
}
void strcatI(U8 *buf, int i)
{
	U8 z[10];
	itoa(i,z,10);
	strcat(buf,z);
}

void DeleteChars(U8 *buffer, U16 len, U8 *pos, U8 anz)
{
	U8 *s;
	U8 *d;
	U8 c;

	s = pos+anz;
	d = pos;
	while ((c=*s++) != 0)
		*d++=c;
	*d = 0;

}

void InsertChar(U8 *buffer, U16 len, U16 pos, U8 ch)
{
	U8 *s;
	U16 slen,i;

	_txt("InsertChar len="); _addi(len); _add(" pos = "); _addi(pos); _show();
	slen = strlen(buffer);
	s = buffer+slen+1;
	for (i=slen+1;i>pos;i--)
		buffer[i] = buffer[i-1];
	buffer[pos] = ch;
}
U16 linelen(U8 *buffer)
{
	U8 *b = buffer,c;
	while ((c = *buffer++) != 0)
		if (c == 10 || c == 13) 
			break;
	return buffer-b;
}

void linecpy(U8 *d, U8 *s)
{
	U8 c;
	while ((c = *s++) != 0)
	{
		if (c == 10 || c == 13) break;
		*d++ = c;
	}
}

U8* linecpy2(U8 *d, U8 *s)
{
	U8 c;
	while ((c = *s++) != 0)
	{
		*d++ = c;
	}
	return d;
}

void copyline(U8 *d, U8 *s,U16 size)
{
	U8 c;

	size-=2;
	while ((c = *s++) != 0)
	{
		if (c == 10||c==13) break;
		if (size-- <= 0) break;
		*d++=c;
	}
	*d++=0;
}

void MoveNextLine(U8 **ptr)
{
	U8 *p = *ptr,c;
	while ((c = *p++) != 0)
	{
		 if (c == 13)
		 {
			 if (*p == 10) p++;
			 *ptr = p;
			 return;
		 }
		 if (c == 10){
			 *ptr = p;
			 return;
		 }
	}
	*ptr = p;
}

void MovePrevLine(U8 **pBuffer, U8 *top)
{
	U8 *p,cnt;
	p = *pBuffer;
	cnt = 2;
	while (p > top)
	{
		if (*p == 13)
		{
			cnt--;
			if (cnt == 0)
			{
				if (p[1] == 10) p++;
				p++;
				break;
			}
		}
		p--;
	}
	*pBuffer = p;
}


U8 MarkLineEnd(U8 *buffer, U8 **pSave)
{
	U8 c;
	while ((c = *buffer++) != 0)
	{
		if (c == 10 || c == 13) {
			*pSave = buffer-1;
			buffer[-1] = 0;
			return c;
		}
	}
	*pSave = 0;
	return 0;
}

void DisplayLine(U16 x, U16 y, U8 ScrollX, U8 *buffer)
{
	U16 width;
	U16 len,i;
	U8  *line,*p,c,save,*pSave;
	p = NULL;
	save = MarkLineEnd(buffer,&pSave);
	line = buffer+ScrollX;
	width = GetTextWidth(line);
	while (width > 240)
	{
		len = strlen(buffer)-1;
		p = buffer + len;
		for (i=len;i>=ScrollX;i++)
		{
			width-=GetCharWidth(*p--);
			if (width <= 239)
				break;
		}
	}
	if (p != NULL)   { c = *p; *p = 0; }
	WriteText(1,line,x,y);
	if (p != NULL)   { *p = c; }
	if (pSave != NULL)
		*pSave = save;
}

void InitEditor(ICEditor *editor)
{
	memset(editor,0,sizeof(ICEditor));
	editor->w = LCD_WIDTH;
	editor->h = LCD_HEIGHT-24;

}

U16 InputEditor(U8 *buffer, U8 size, S16 x, S16 y, S16 *pCursor,ICEditor *Editor)
{
	U16 w;
	U16 key;
	S16 Cursor;
	U8  c;
	U8 i;
	U8  finish;
	Cursor = *pCursor;
	if (Cursor > strlen(buffer)) 
		Cursor = strlen(buffer)-1;
	if (Cursor < 0) Cursor = 0;

	finish = FALSE;
	Editor->LineModified = FALSE;
	MaskSymKey(1);
	
	while (!finish)
	{
		strcpy(gTempBuf,"Line: ");
		strcatI(gTempBuf,Editor->line);
		strcat(gTempBuf," TopLine: ");
		strcatI(gTempBuf,Editor->TopLine);
		strcat(gTempBuf," Scrline=");
		strcatI(gTempBuf,y / 12);
		strcat(gTempBuf,"  ");
		if (Editor->InsertMode)
			strcat(gTempBuf,"Ins");
		else
		    strcat(gTempBuf,"Ovr");
		FillBlock(1,0,160-12,239,12);
		WriteText(0,gTempBuf,0,160-12);


		SetCursorStatus(0);
		FillBlock(0,x,y,239,NORMAL_FONT_HEIGHT);
		DisplayLine(x,y,Editor->ScrollX,buffer);

		c = buffer[Cursor+Editor->ScrollX];
		buffer[Cursor] = 0;
		while (TRUE)
		{
			w = GetTextWidth(buffer+Editor->ScrollX);
			if (w < LCD_WIDTH)  break;
			if (Editor->OnScroll == NULL) break;
			Editor->OnScroll(10,Editor);
		}
		buffer[Cursor] = c;
		SetCursorPosition(x+w,y);
		w = GetCharWidth(buffer[Cursor]);
		if (w == 0) w = 3;
		if (Editor->InsertMode) w = 2;
		SetCursorShape(w,12);
		SetCursorStatus(1);
		key = GetKey();
		w = strlen(buffer);
		switch(key)
		{ 
			case LEFT:			if (Cursor>0) Cursor--;	
								if (Cursor < Editor->ScrollX && Editor->OnScroll != NULL)
									Editor->OnScroll(-10,Editor);
								break;
			case RIGHT:			if (Cursor<w) Cursor++;	
		
								break;
			case DEL:			if (Cursor >= strlen(buffer)) 
								{	
									*pCursor = Cursor;
									return DEL;
								}
								for (i = Cursor;i<size-1;i++)
									buffer[i] = buffer[i+1];
								buffer[Cursor-1] = 0;
								Editor->LineModified = TRUE;
								break;
			case INS:			Editor->InsertMode = Editor->InsertMode ? 0 : 1; break;
			case ENTER:			
			case UP:
			case DOWN:
			case MENU:
			case PGUP:
			case ESC:
			case PGDN:			*pCursor = Cursor;
								return key;


			case BKSP:			if (Cursor>0) {				         
									for (i = Cursor-1;i<strlen(buffer);i++)
										buffer[i] = buffer[i+1];
									Cursor--;
								} else {
									*pCursor = Cursor;
									return BKSP;
								}
								Editor->LineModified = TRUE;
								break;
			default:			if (Editor->InsertMode) {
									_txt("InsertMode cursor=");
									_addi(Cursor); _show();
									InsertChar(buffer,255,Cursor,key);
								} else
									buffer[Cursor] = key;
								Cursor++;
								Editor->LineModified = TRUE;
								if (Cursor>w) buffer[Cursor] = 0;
		}
	}
}

U16  GetLineCount(U8 *buffer)
{
	U16 lines = 0;
	while (*buffer != 0)
	{
		if (*buffer == 10) lines++;
		if (*buffer == 0) break;
		buffer++;
	}
	return lines;
}
U8* FindLine(U8 *buffer, U16 line)
{
	U8 c,*p,*prev;
	p = buffer;
	prev = buffer;
	if (line == 0) return buffer;
	while ((c = *p++) != 0)
	{
		if (c == 13) {
			if (*p == 10) p++;
			prev = p;
			line--;
			if (line == 0) 
				return p;
		}
	}
	return prev;
}

void SaveLine(ICEditor *editor, U8 *pBuffer)
{
	U16 lenLine, lenCurLine;
	U8  *s,*d,*m,c;
	U16 diff,len;
	gFileDirty = TRUE;

	lenLine    = strlen(pBuffer)+1;
	lenCurLine = linelen(editor->pLine);
/* if the current line is bigger, move the text */
	if (lenLine > lenCurLine) 
	{
		diff = lenLine-lenCurLine;
		len  = strlen(editor->buffer);
		s = editor->buffer + len;
		d = editor->buffer + len + diff;
		if (d > editor->buffer + editor->bufferSize) 
			return;

		m = editor->pLine;
		while (s > m)
		{
			*d = *s; s--; d--;
		}
	}
	if (lenCurLine > lenLine)
	{
		diff = lenCurLine-lenLine;
		s = editor->pLine+diff;
		d = editor->pLine;
		while ((c = *s++) != 0)
			*d++ = c;
		*d++=c;
	}
	d = linecpy2(editor->pLine,pBuffer);
	*d++=13;
	*d++=10;
}




			 


void DisplayPage(ICEditor *editor)
{
   int x,y;
   U8  *p;
   x = editor->x;
   y = editor->y;
   p = editor->pTopLine;
   FillBlock(0,editor->x,editor->y,editor->w,editor->h);
   while (y <= editor->h+editor->y)
   {
	   DisplayLine(x,y,editor->ScrollX,p);
	   y += 12;
	   MoveNextLine(&p);
   }
}

void EditorPrevLine(ICEditor *editor)
{
	U8 *p,*b,cnt;
	b = editor->buffer;
	p = editor->pLine;
	cnt = 2;
	while (p > b)
	{
		if (*p == 13)
		{
			cnt--;
			if (cnt == 0)
			{
				if (*p==13) p++;
				if (*p==10) p++;
				break;
			}
		}
		p--;
	}
	editor->pLine = p;
	editor->line--;
}




void EditorNextLine(ICEditor *editor)
{
	U8 *p;
	editor->line++;
	p = editor->pLine;
	MoveNextLine(&p);
//	p = editor->pLine + strlen(editor->pLine);
	if (editor->pLine - editor->buffer < editor->bufferSize)
	{
		editor->pLine = p;
	}
}

void RecalcLines(ICEditor *editor)
{
	editor->pTopLine = FindLine(editor->buffer,editor->TopLine);
	editor->pLine    = FindLine(editor->buffer,editor->line);
}

void ICEditFile(ICEditor *editor, U8 *buffer, U16 length, U16 line)
{
	S16      key;
	U8		 finish;
	U8		 Cursor;
	U8       screenLine;
	U8		 r;
	S16		 x,y;
	U8      *p;
	U8		NeedsRepaint;
	
	editor->buffer		= buffer;
	editor->bufferSize	= length;
	editor->line		= line;
	editor->pLine		= FindLine(buffer,line);
	editor->TopLine		= line;
	editor->pTopLine	= editor->pLine;

	Cursor = 1;
	
	finish = FALSE;
	DisplayPage(editor);
	x = editor->x;
	y = editor->y;
	screenLine = 1;
	while (!finish)
	{


#ifdef _WIN32
		p = FindLine(buffer,editor->line);
		if (p != editor->pLine)
		{
//			_ASSERTE(FALSE);
				editor->pLine = p;
		}
#endif
		editor->pLine = FindLine(buffer,editor->line);
		
		NeedsRepaint = FALSE;
		copyline(gPrintBuf,editor->pLine,PRINTBUFSIZE-1);
		key = InputEditor(gPrintBuf,PRINTBUFSIZE-1,x,y,&Cursor,editor);
		if (key == DEL) {
			if (editor->LineModified)
				SaveLine(editor,gPrintBuf);
			DeleteChars(editor->buffer,editor->bufferSize,editor->pLine + Cursor,2);
			NeedsRepaint = TRUE;
			editor->LineModified = FALSE;
		}
		if (key == ENTER && editor->InsertMode) {
			editor->LineModified = TRUE;
			InsertChar(gPrintBuf,PRINTBUFSIZE-1,Cursor,10);
			InsertChar(gPrintBuf,PRINTBUFSIZE-1,Cursor,13);
		}
		if (editor->LineModified)
				SaveLine(editor,gPrintBuf);
		if (key == ENTER && editor->InsertMode) {
			RecalcLines(editor);
			NeedsRepaint = TRUE;
			
		}
		if (key == ENTER) {
			Cursor = 0;
			key = DOWN;
		}
		switch(key)
		{
			case PGDN:	    if (editor->TopLine + (editor->h / NORMAL_FONT_HEIGHT) < GetLineCount(editor->buffer))
							{
								editor->TopLine += (editor->h / NORMAL_FONT_HEIGHT);
								editor->line += (editor->h / NORMAL_FONT_HEIGHT);
								RecalcLines(editor);
								NeedsRepaint = TRUE;
							}
							break;

			case PGUP:	    if (editor->TopLine > (editor->h / NORMAL_FONT_HEIGHT))
							{
								editor->TopLine -= (editor->h / NORMAL_FONT_HEIGHT);
								editor->line -= (editor->h / NORMAL_FONT_HEIGHT);
								if (editor->TopLine < 1) editor->TopLine = 1;
								if (editor->line    < 1) editor->line = 1;
								RecalcLines(editor);
								NeedsRepaint = TRUE;
							}
							break;
			case MENU:		if (editor->OnMenu != NULL) {
								editor->EventEditorReturn = EDITOR_NULL;
								r = editor->OnMenu(editor);
								if (editor->EventEditorReturn == EDITOR_EXIT) finish=TRUE;
							}

			case DOWN:		EditorNextLine(editor);
							y += NORMAL_FONT_HEIGHT;
							if (y >= editor->h + editor->y) {
								y -= NORMAL_FONT_HEIGHT;
								editor->TopLine++;
								editor->pTopLine = FindLine(buffer,editor->TopLine);
								NeedsRepaint = TRUE;
							}
							break;
			case UP:	    if (editor->line > 0)
							{
								EditorPrevLine(editor);
								y -= NORMAL_FONT_HEIGHT;
								if (y <=  editor->y && editor->TopLine > 0) {
									y += NORMAL_FONT_HEIGHT;
									editor->TopLine--;
									/*MovePrevLine(editor->pTopLine,editor->buffer);*/
									editor->pTopLine = FindLine(buffer,editor->TopLine);
									NeedsRepaint = TRUE;
									
								}
							}
							break;
			case ESC:		editor->EventReturnCode = 0;
							finish = TRUE;					break;
		}
		if (NeedsRepaint)
			DisplayPage(editor);
	}
}

/* We use the memory starting at gHeapTop until gStackPointer as temporary
   memory
*/

U8 CompareFilter(U8 *Filter, U8 *Ext)
{
	U8 *s;
	U8 c;
	U8 r;
	s = Filter;
	while (TRUE)
	{
		c = *s++;
		if (c == '|' || c == 0) {
			s[-1] = 0;
			r = ic_stricmp(Filter,Ext) == 0;
			s[-1] = c;
			Filter = s;
			if (r || c == 0) return r;
			s++;
		}
	}
}

U8 FSEL_ReadInfos(U8 *Directory, FILE_INFO *pInfo,U8 dir, U8 *Filter)
{
	FILE_IDEN fileIden;
	FILE_INFO *infos;
	S16 hr;
	S16 count;
	S16 i;
	infos = pInfo;
	hr = mOpenDirectory(Directory,OPEN_EXISTING,&fileIden);
	if (hr == MMC_ERROR_SUCCESS)
	{
		hr = mGetDirectorySubItemNum(&fileIden,&count);
		if (hr == MMC_ERROR_SUCCESS)
		{
			for (i=1;i<=count;i++)
			{
				if ((U8*)(pInfo+1) > gStackPtr) break;
				mGetDirectorySubItem(&fileIden,pInfo,i);
				if (dir) 
				{
					if ((pInfo->byAttribute & AttrDirectory) != 0)
						pInfo++;
				}
				else
				{
					if (Filter == NULL)
						pInfo++;
					else
					    if (CompareFilter(Filter,pInfo->cExtName))
							pInfo++;
				}
				
			}
		}
		mCloseDirectory(&fileIden);
	}
	return pInfo-infos;
}

FSEL_ShowLine(FILE_INFO *pInfo, int y, U8 selected)
{
	char buf[20];
		gTerm.curX = 3*4;
		gTerm.curY = (y+3)*12;
		ic_puts(&gTerm,pInfo->cFileName);
		gTerm.curX = 13*4;
		ic_puts(&gTerm,pInfo->cExtName);
		gTerm.curX = 18*4;
		if ((pInfo->byAttribute & AttrDirectory) == 0)
		{
			ltoa(&pInfo->dwFileSize,buf,10);
			ic_puts(&gTerm,buf);
		}
     	pInfo++;
		if (selected)
			ShadowEdgeRect(4,(y+3)*12,230,12);

}
void FSEL_ShowPage(FILE_INFO *info,S16 start, S16 max,U8 *FileName, U8 *Title)
{
	U8  x,y;
	U8  i;
	x = 0; y = 12*3;
	FillScreen(0);
	FillBlock(1,0,0,LCD_WIDTH-1,24);
	WriteText(0,Title,0,0);
	WriteText(0,FileName,0,12);
	gTerm.curX = 0;
	gTerm.curY = 12;
/*	ic_puts(&gTerm,FileName); */
	for (i=0;i<FILES_PER_PAGE;i++)
	{
		if (start + i > max) break;
		FSEL_ShowLine(info+i,i,FALSE);	
	}
	
}
FILE_INFO *FSEL_SelectDirectory(FILE_INFO *pInfo, S16 count,U8 *FileName, U8 *Title)
{
	S16 id,id1,pageTop,key;
	ic_cls(&gTerm);
	pageTop = 0;
	id = 0;
	FSEL_ShowPage(pInfo,pageTop,count,FileName,Title);
	while (TRUE)
	{
		
		BackupLcd(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);
		if (id >= pageTop && id <= pageTop+11) 
			FSEL_ShowLine(pInfo+id,id-pageTop,TRUE);
		key = GetKey();
		RestoreLcd(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);
		
		id1 = id;
		switch(key)
		{
			case ENTER:		return pInfo+id;
			case ESC:		return NULL;
			case DOWN:		if (id < count) id ++;   break;
			case UP:		if (id >   0) id --;   break;
		}
		if (id1 != id)
		{
			if (id < pageTop) {
				pageTop -= FILES_PER_PAGE;
				if (pageTop < 1) {pageTop = 0; id = 0; }
				FSEL_ShowPage(pInfo+pageTop,pageTop,count,FileName,Title);
			}
			if (id > pageTop+FILES_PER_PAGE-1)
			{
				pageTop += FILES_PER_PAGE;
				if (pageTop > count) pageTop = count-2;
				FSEL_ShowPage(pInfo+pageTop,pageTop,count,FileName,Title);
			}

		}	
	}
	return NULL;

}

U8 SelectFile(U8 *Directory, U8 *FileName,U8 *Filter, U8 *Title)
{
	FILE_INFO *infos,*pInfo;
	S16        count;
	U8		   *p;
	infos = (FILE_INFO*)gHeapTop;
	while (TRUE)
	{
		count = FSEL_ReadInfos(FileName,infos,TRUE,NULL);
		count +=FSEL_ReadInfos(FileName,infos+count,FALSE,Filter);
		pInfo = FSEL_SelectDirectory(infos,count,FileName,Title);
		if (pInfo == NULL) return 0;
		if ((pInfo->byAttribute & AttrDirectory) != 0) {
			if (strcmp(pInfo->cFileName,".") != 0)
			{
				if (strcmp(pInfo->cFileName,"..") == 0) 
				{
					p = FileName+strlen(FileName)-1;
					while (*p != '\\' && p > FileName) p--;
					*p = 0;
				} else
				{
					strcat(FileName,"\\");
					strcat(FileName,pInfo->cFileName);
					if (pInfo->cExtName[0] != 0) {
						strcat(FileName,".");
						strcat(FileName,pInfo->cExtName);
					}
				}
			}
		}
		else
		{
			strcat(FileName,"\\");
			strcat(FileName,pInfo->cFileName);
			if (pInfo->cExtName[0] != 0) 
			{
				strcat(FileName,".");
				strcat(FileName,pInfo->cExtName);
			}
			return 1;
		}
	}
}