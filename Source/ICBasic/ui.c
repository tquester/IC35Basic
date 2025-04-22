
#include "ic35sdk.h"
#include "data.h"
#include "compiler.h"
#include "vm.h"
#include "uitool.h"
#include "icstdioq.h"
#include "icmemory.h"
#include "imgdata.h"

#define UI_EXIT		          0
#define UI_COMPILE			100
#define UI_COMPILE_RUN		101
#define UI_COMPILE_DEBUG	102
#define UI_SAVE				103
#define UI_OPEN				104


void CompileFile();
void CompileEditor(U8 ui_Aktion);
void DebugFile();
void SaveBasic();
void NewFile();

void HelpToken();
void Disass(U8 *adr, U8 *end, U8 printit);
void CallEditor(ICEditor *editor, U8 *buffer, U16 length, U16 line);

static void _print2(char *s)
{
	strcpy(gPrintBuf,s); 
	ic_puts(&gTerm,gPrintBuf);
}

static void _print2i(S16 i)
{
	itoa(i,gPrintBuf,10);
	ic_puts(&gTerm,gPrintBuf);
}

static void _print2h(U16 i)
{
	utohex(gPrintBuf,i);
	ic_puts(&gTerm,gPrintBuf);
}

void Msg(char *s)
{
	
	U16 key;

	ic_puts(&gTerm,s);
	ic_puts(&gTerm,"\n");
	key = GetKey();
}

void Msg1(char *s)
{
	
/*	U16 key; */

	ic_puts(&gTerm,s);
	ic_puts(&gTerm," ");
/*	key = GetKey(); */
}

void CheckMemory()
{
	if (gSettings.Size_Code == 0)		BTN2("Codesize = 0");
	if (gSettings.Size_Sourcecode == 0) BTN2("Sourcecodesize = 0");
	if (gSettings.Size_Variables == 0)	BTN2("Varsize = NULL");
	if (gSettings.Size_Heap == 0)		BTN2("Heap = NULL");

}


void Input(U8 *buffer, U8 size, S16 x, S16 y)
{
	S16 w;
	U16 key;
	U16 Cursor;
	U8  c;
	U16 i;
	U8  finish;
	Cursor = strlen(buffer);
	finish = FALSE;
	
	while (!finish)
	{

		SetCursorStatus(0);
		w = GetTextWidth(buffer);
		FillBlock(0,x,y,239,12);
		WriteText(1,buffer,x,y);

		c = buffer[Cursor];
		buffer[Cursor] = 0;
		w = GetTextWidth(buffer);
		buffer[Cursor] = c;

		SetCursorPosition(x+w,y);
		w = GetCharWidth(buffer[Cursor]);
		if (w == 0) w = 3;
		SetCursorShape(w,12);
		SetCursorStatus(1);
		key = GetKey();
		w = strlen(buffer);
		switch(key)
		{
			case 13: finish = TRUE;
					 break;
			case 1:  if (Cursor>0) Cursor--;	break;
			case 2:  if (Cursor<w) Cursor++;	break;
			case 7:  for (i = Cursor;i<size-1;i++)
						 buffer[i] = buffer[i+1];
					 buffer[w-1] = 0;
					 break;

			case 8:  if (Cursor>0) {
				         
						 for (i = Cursor;i<size-1;i++)
							buffer[i] = buffer[i+1];
						 buffer[w-1] = 0;
						 Cursor--;
					 }

					 break;
			default: buffer[Cursor] = (U8)key;
					 Cursor++;
					 if (Cursor>w) buffer[Cursor] = 0;
		}
	}
}

void InputLine(U8 *buffer, U8 size)
{
	Input(buffer,size,gTerm.curX,gTerm.curY);
	gPrintBuf[0] = '\n';
	gPrintBuf[1] = 0;
	ic_puts(&gTerm,gPrintBuf);
}

void SaveChanges()
{
  if (gFileLoaded && gFileDirty)
  {
	  strcpy(gTempBuf,"Save changes to ");
	  strcat(gTempBuf,gFileName);
	  strcat(gTempBuf,"?");
	  if (StringDialogBox(gTempBuf,YESNO) == IDYES)
	  {
		  SaveBasic();
	  }
  }
}

void SaveCode()
{
	U8 *s,*p,c;
	U8 found;
	S16 r;
	U16 size;
	FILE_IDEN iden;
	found = FALSE;
	_print2("Saving code\n");
	strcpy(gTempBuf,gFileName);
	s = gTempBuf;
	p = NULL;
	_print2("\n");
	while ((c = *s++) != 0)
	{
		if (c == '.') p = s;
	}
	if (p != NULL)
	{
		    found = TRUE;
			strcpy(p,"BAC");
			size = gCodePtr-gCode;
			_print2("Saving binary file to\n");
			_print2(gTempBuf);
			_print2("(");
			_print2i(size);
			_print2(" ) bytes\n");
			r = mOpenFile(gTempBuf,CREATE_ALWAYS,&iden);
			if (r != MMC_ERROR_SUCCESS) found = FALSE;
			r = mWriteToFile(&iden,gCode,size);
			if (r != MMC_ERROR_SUCCESS) found = FALSE;
			mCloseFile(&iden);
			if (!found) 
				_print2("Error saving file\n");
	} else
		_print2("\n.BAS not found in filename\n");
}

void NewFile()
{
	strcpy(gPrintBuf,"File name for new program");
	MaskSymKey(1);
	
	InputControl(gPrintBuf,gFileName,0,64,  
			       20,40,200,0);
}
void LoadBasic()
{
	FILE_IDEN iden;
	U16 r,read;
	gFileLoaded = TRUE;
	gFileDirty = FALSE;

/*	strcpy(gTempBuf,"MMCard1\\BASIC.BAS"); */

	r = mOpenFile(gFileName,OPEN_EXISTING, &iden);
	if (r != MMC_ERROR_SUCCESS) {
		strcpy(gTempBuf,"File not found");
		StringDialogBox(gTempBuf,OK);
		return;
	}

	r = mReadFromFile(&iden,gSource,(U16)iden.dwFileSize,&read);
	if (r != MMC_ERROR_SUCCESS) {
		Msg("Error reading file");
		return;
	}
	gSource[iden.dwFileSize] = 0;

	mCloseFile(&iden);
}

void SaveBasic()
{
	FILE_IDEN iden;
	S16 r;

	r = mOpenFile(gFileName,CREATE_ALWAYS, &iden);
	if (r != MMC_ERROR_SUCCESS) {
		Msg("Need \\BASIC.BAS to run");
		return;
	}

	r = mWriteToFile(&iden,gSource,(U16)strlen(gSource)+1);
	if (r != MMC_ERROR_SUCCESS) {
		Msg("Error reading file");
		return;
	}

	mCloseFile(&iden);
}

void LoadAndRun(U8 *filename)
{
	S16 r;
	U16 read;
	
	FILE_IDEN iden; 

	SaveChanges();
	
	ic_InitalAllocMemory();
	r = mOpenFile(filename,OPEN_EXISTING,&iden);
	if (r == MMC_ERROR_SUCCESS)
	{
		r = mReadFromFile(&iden,(U16)gCode,iden.dwFileSize,&read);
		if (r == MMC_ERROR_SUCCESS)
		{
			mCloseFile(&iden);
			ExecuteVM(gCode);
		}
	}
	*gFileName = 0;
	gFileLoaded = FALSE;
}

void LoadBasicFile()
{
	U8 *p;
	strcpy(gTempBuf,"MMCard1\\ICBasic");
	strcpy(gStack,"MMCard1\\ICBasic");
	strcpy(gTempBuf+64,"BAS|BAC");
	strcpy(gTempBuf+128,"Select file to run");
	if (SelectFile(gTempBuf, gStack, gTempBuf+64,gTempBuf+128) != 0)
	{
		p = gStack+strlen(gStack)-1;
		while (p > gStack)
		{
			if (*p == '.')
			{

				if (strcmp(p,".BAC") == 0) {
					LoadAndRun(gStack);
				}
				else
				{
					strcpy(gFileName,gStack);
					gFileLoaded = TRUE;
					gFileDirty  = FALSE;
					LoadBasic();
				}
				break;

			}
			p--;
		}
		
	}
}

void SaveBasicFile()
{
	SaveBasic();
	strcpy(gTempBuf,"Source code saved");
	StringDialogBox(gTempBuf,2);
}

U8 OnEditMenu(ICEditor *editor)
{
	
	U16 r;
	U8 temp[160];
	strcpy(temp,"010File^020Editor^030Compiler^040Info^#100Open^110Save^");
	strcat(temp,"130Exit^240Mark^250Cut^200Copy^210Paste^300Compile^310Compile/Run^320Debug^400About^");
	strcat(temp,"410Options^"); 
	r = StringMenu(temp,0);

	switch(r) 
	{
		case 0:      editor->EventEditorReturn = EDITOR_EXIT;
					 editor->EventReturnCode   = UI_OPEN;
					 break;
		case 30:	 editor->EventEditorReturn = EDITOR_EXIT;
					 editor->EventReturnCode   = UI_SAVE;
					 break;
		case 90:	 editor->EventEditorReturn = EDITOR_EXIT;
					 editor->EventReturnCode   = 0;
					 break;
		case 0x3c:   /* File Exit */
					 editor->EventEditorReturn = EDITOR_EXIT;
					 editor->EventReturnCode   = UI_EXIT;
					 break;
		case 0x708:  /* Compiler, Compile */
					 editor->EventEditorReturn = EDITOR_EXIT;
					 editor->EventReturnCode   = UI_COMPILE;
					 break;
		case 0x726:  /* Compiler, Compile */
					 editor->EventEditorReturn = EDITOR_EXIT;
					 editor->EventReturnCode   = UI_COMPILE_RUN;
					 break;
		case 0x744:  /* Compiler, Compile */
					 editor->EventEditorReturn = EDITOR_EXIT;
					 editor->EventReturnCode   = UI_COMPILE_DEBUG;
					 break;
	}
	
	return r;

}

/* -----------------------------------------------------------------------------
   -- Main editor                                                             --
   -----------------------------------------------------------------------------
*/

void EditFile()
{
	U16 line;
	ICEditor editor;
	InitEditor(&editor);
	editor.OnMenu = OnEditMenu;
	line = 0;
	while (TRUE)
	{
		FillScreen(WHITE);
		CallEditor(&editor,gSource,gSettings.Size_Sourcecode-1,line);
		
		if (editor.EventReturnCode == UI_EXIT) break;
		ic_cls(&gTerm);

		switch(editor.EventReturnCode)
		{
			case UI_COMPILE_DEBUG:
			case UI_COMPILE_RUN:
			case UI_COMPILE:
				 CompileEditor(editor.EventReturnCode);
				 if (gErrorCode != 0) line = gLine-1;
				 break;
			case UI_OPEN:
				 SaveChanges(); LoadBasicFile();
				 break;
			case UI_SAVE:
				 SaveBasicFile();
				 break;


		}
	}
}
void RunFile();




void CompileFile()
{
	U16 key;
	FillScreen(WHITE);
	ic_cls(&gTerm);
	_print2("Include debug info (j/n)?\n");
	key = GetKey();
    gCompilerInsertDebug = TRUE;
	if (key == 'n') gCompilerInsertDebug = FALSE;

	ic_initMemory();
	InitCompiler();
	gCompilerShowListing = FALSE;
	gPass = 1;
	_print2("Pass 1\n");
	gLastVar = 0;
	Compile();
	ReportError();
	if (gErrorCode == 0)
	{

		gSourcePtr = gSource;
		gCompilerShowListing = FALSE;
		gPass = 2;
		_print2("Pass 2\n");
		Compile();
		ReportError();
		if (gErrorCode == 0)
		{
			_print2("Compiled ");
			_print2i(gLine);
			_print2(" lines (");
			_print2i(gSourcePtr-gSource);
			_print2(" bytes)\nInto ");
			_print2i(gCodePtr-gCode);
			_print2(" bytes.\n");
			_print2("Variable space used: ");
			_print2i(gLastVar);
			_print2(" (");
			_print2i(gLastVar*sizeof(Variable));
			_print2(" bytes)\n");
			_print2("Memory used: ");
			_print2i(gMemoryPtr-gHeap);
			_print2(" bytes\n");
			_print2("Variables allocated ");
			_print2i(gVariablesSize);
			_print2(" bytes\n");
			SaveCode();
			

			_print2("\n\nPress 'r' to run");
			key = GetKey();
			if (key == 'r' || key == 'R') 
				RunFile();
			return;
		}

	} 
	_print2("\nPress any key");
	GetKey();

}

void CompileEditor(U8 ui_Aktion)
{
	U16 key;
	ic_cls(&gTerm);
	if (ui_Aktion == UI_COMPILE_RUN)
		gCompilerInsertDebug = TRUE;
	else
		gCompilerInsertDebug = TRUE;

	ic_initMemory();
	InitCompiler();
	gCompilerShowListing = FALSE;
	gPass = 1;
	_print2("Pass 1\n");
	gLastVar = 0;
	Compile();
	ReportError();
	if (gErrorCode != 0)
	{
		_print2("\nPress any key");
		GetKey();
	}
	else
	{

		gSourcePtr = gSource;
		gCompilerShowListing = FALSE;
		gPass = 2;
		_print2("Pass 2\n");
		Compile();
		ReportError();
		if (gErrorCode == 0)
		{
			switch(ui_Aktion)
			{
				case UI_COMPILE:
					_print2("Compiled ");
					_print2i(gLine);
					_print2(" lines (");
					_print2i(gSourcePtr-gSource);
					_print2(" bytes)\nInto ");
					_print2i(gCodePtr-gCode);
					_print2(" bytes.\n");
					_print2("Variable space used: ");
					_print2i(gLastVar);
					_print2(" (");
					_print2i(gLastVar*sizeof(Variable));
					_print2(" bytes)\n");
					_print2("Memory used: ");
					_print2i(gMemoryPtr-gHeap);
					_print2(" bytes\n");
					_print2("Variables allocated ");
					_print2i(gVariablesSize);
					_print2(" bytes\n");
					SaveCode();

					_print2("\n\nPress 'r' to run");
					key = GetKey();
					if (key == 'r' || key == 'R') 
						RunFile();
					break;
			case UI_COMPILE_RUN:
				    RunFile();
					break;
			case UI_COMPILE_DEBUG:
				    DebugFile();
					break;
			}
		}
	} 
}



void RunFile1()
{
	_print2("\n\n\n");
	gLastVar = 0;
	ic_cls(&gTerm);
	if (gErrorCode != 0)
	{
		_print2("The file contains errors, please corret and try again");
	} else {
		_print2("Staring VM\n");
		_print2("Clearing variables\n");
		memset(gVariables,0,gSettings.Size_Variables-1);
		
		if (gDebug==2) {
			_print2("Execut debug\n");
			ExecuteVMDebug(gCode);
		} else {
			_print2("Executing VM\n");
			ExecuteVM(gCode);
		}
		strcpy(gPrintBuf,"Executed. Last statement = ");
		itoa(gCodePtr-gCode,gTempBuf,10);
		strcat(gPrintBuf,gTempBuf);
		ic_puts(&gTerm,gPrintBuf);
		GetKey();
	}
}

void RunFile()
{
	gDebug = 0;
	RunFile1();
}

void DebugFile()
{
	gDebug=1;
	RunFile1();
}

void DebugAssFile()
{
	gDebug=2;
	RunFile1();
}

void DisassembleFile()
{
	U16 key;
	ic_cls(&gTerm);
	strcpy(gPrintBuf,"\nOK Press 'a' to view assembly code");
	ic_puts(&gTerm,gPrintBuf);
	key = GetKey();
	if (key == 'a') {
		ic_cls(&gTerm);
		Disass(gCode+sizeof(CodeHeader),gCodePtr,1);
		key = GetKey();
	} else
	{
		_print2("\nWriting assembly output\n");
		Disass(gCode+sizeof(CodeHeader),gCodePtr,0);
	}

}

void About()
{
	U8 *p1,*p2;
	U16 m1,m2;
	ic_cls(&gTerm);
	_print2("\n\nICBasic compiler Version 0.01\n");
	_print2("(C) 2002 Thomas Quester\n");
	_print2("On IC35 this program is freeware\n");
	_print2(" (Ports on Palm and CE coming soon)\n\n");
	_print2("For questions and comments, write to\n");
	_print2("ICBasic@outlookfolders.com\n\n");
	_print2("Memory free:");
	p1 = (U8*)&_data_Begin;
	p2 = (U8*)&_data_End;
	_print2i((U16)(p2-p1));
	_print2("\n");
	m1 = gMemory;
	m2 = 0xC000;

	ltohex(gTempBuf,(U16)m1);
	_print2(gTempBuf);
	_print2("\n");
	_print2("Max size of memory:");
	_print2i(m2-m1);
	_print2("\n");
}

void Options()
{
	ic_cls(&gTerm);

	_print2("\nMemory usuage\n\n");
	_print2("Source code\n");
	_print2("Heap\n");
	_print2("Code\n");
	_print2("Stack\n");
	_print2("Variables\n");
	_print2("Total\n");

	CheckMemory();
	ic_gotoxy(&gTerm,80,2*12);			_print2("Size");
	ic_gotoxy(&gTerm,120,2*12);			_print2("Start");
	ic_gotoxy(&gTerm,160,2*12);			_print2("Stop");
	ic_gotoxy(&gTerm,80,3*12);   		_print2i(gSettings.Size_Sourcecode);
	ic_gotoxy(&gTerm,120,3*12);			_print2h(gSource);
	ic_gotoxy(&gTerm,160,3*12);			_print2h(gSourcePtr);

	ic_gotoxy(&gTerm,80,4*12);   		_print2i(gSettings.Size_Heap      );
	ic_gotoxy(&gTerm,120,4*12);			_print2h(gHeap);
	ic_gotoxy(&gTerm,160,4*12);			_print2h(gHeapTop);
	
	ic_gotoxy(&gTerm,80,5*12);   		_print2i(gSettings.Size_Code      );
	ic_gotoxy(&gTerm,120,5*12);			_print2h(gCode);
	ic_gotoxy(&gTerm,160,5*12);			_print2h(gCodePtr);
	
	ic_gotoxy(&gTerm,80,6*12);   		_print2i(gSettings.Size_Stack     );
	ic_gotoxy(&gTerm,120,6*12);			_print2h(gStack);
	ic_gotoxy(&gTerm,160,6*12);			_print2h(gStack+gSettings.Size_Stack);
	
	ic_gotoxy(&gTerm,80,7*12); 			_print2i(gSettings.Size_Variables );
	ic_gotoxy(&gTerm,120,7*12);			_print2h(gVariables);

	ic_gotoxy(&gTerm,80,11*12);			_print2("Ram Banks: ");
	ic_gotoxy(&gTerm,120,11*12);		_print2i(gOrgRamBank);
	ic_gotoxy(&gTerm,160,11*12);		_print2("Grafik ");
										_print2i(BANK_IMAGES);
	
	ic_gotoxy(&gTerm,80,8*12);   		
	_print2i(	gSettings.Size_Sourcecode		+
				gSettings.Size_Heap      		+
				gSettings.Size_Code      		+
				gSettings.Size_Stack     		+
				gSettings.Size_Variables);
	GetKey();
}

void Anykey()
{
	_print2("Press any key\n");
	GetKey();

}

U16 M1()
{
	char TBuf[200];  
	U16 r;
#ifdef _WIN32
	strcpy(TBuf,"010File^020Compiler^030Info^#100New^140Open^150Save^160Exit^210Edit^220Compile^230Debug^240Debug Assembler^250Disassemble^260Run^310About^200CompileLoop^320Options^330Tokens^"); 
#else
	strcpy(TBuf,"010File^020Compiler^030Info^#100New^140Open^150Save^160Exit^210Edit^220Compile^230Debug^240Debug Assembler^250Disassemble^260Run^310About^320Options^330Tokens^"); 
#endif
	r = StringMenu(TBuf,0);
	return r;

}
#ifdef _WIN32
void CompileLoop()
{
	S16 r;
	while (TRUE)
	{
		ic_cls(&gTerm);
		_print2("Edit your file in the editor, press yes to compile/run or no to exit");
		strcpy(gTempBuf,"Compile/Run again");
		r = StringDialogBox(gTempBuf,YESNO);
		if ( r == IDNO) break;
		LoadBasic();
		CompileEditor(UI_COMPILE_RUN);
	}
}
#endif

U16 DoMenu()
{
	U16 r;
	U8 ex = FALSE;
	CheckMemory();
	while (ex == FALSE)
	{
		CheckMemory();
		ic_cls(&gTerm);
		CheckMemory();

		_print2("\n\n\n\n\n\n\nICBasic Version 0.4 02/20/2002 (C)2002 Thomas Quester\n");
		_print2("http://www.outlookfolders.de/ic35\n");
		_print2("Email: icbasic@outlookfolders.de\n");
		CheckMemory();
		r = M1();
		CheckMemory();
		//strcpy(gTempBuf,"010File^020Compiler^030Info^#140Open^150Save^160Exit^270Edit^280Compile^290Disassemble^2A0Run^3B0About^");
		//r = StringMenu(gTempBuf,0);
		switch(r)
		{
			case 0:   ic_InitalAllocMemory();
					  NewFile();				break;
			case 30:  SaveChanges(); 
					  ic_InitalAllocMemory();
					  LoadBasicFile();			break;
			case 60:  SaveBasicFile();			break;
			case 90:  ex = TRUE;				break;

			case 930:  CompileFile();			break;
			case 900:  EditFile();				break;
			case 960:  DebugFile();				break;
			case 990:  DebugAssFile();			break;
			case 1020: DisassembleFile();		break;

#ifdef _WIN32
			case 1080: CompileLoop();			break;
#endif
			case 1050: RunFile();				break;
			case 1800: About();	Anykey();		break;
			case 1830: Options();				break;
			case 1860: HelpToken();				break;
		}	
	}
	return r;
}

