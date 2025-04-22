
#include "ic35sdk.h"
#include "data.h"
#include "vm.h"
#include "uitool.h"
#include "ui.h"
#include "compiler.h"
#include "icmemory.h"
#include "icstdioq.h"
#include "imgdata.h"


void cMain(void)
{

	FILE_IDEN iden;
	gFileLoaded = FALSE;
	SetKeyTone(0);
	SetRamBank(STD_BANK);
	gOrgRamBank = GetRamBank();
	/*
	SetRamBank(BANK_IMAGES);
	gImagePtr = gImageData+1;
	gImageTop = gImageData+IMG_SIZE;
	SetRamBank(gOrgRamBank);
    */
	
	SetFontMode(0);
	FillScreen(WHITE);
	BTN("Clearing memory...");
	memset(gMemory,0,MEMORYSIZE-1); 
	/* memset(gMemory,0,10000); */
	

	BTN("InitICTerm");
	InitICTerm(&gTerm,NULL,0);	

	BTN("ic_InitSettings");
	ic_InitSettings();
	BTN("ic_InitalAllocMemory"); 
	ic_InitalAllocMemory();
	ShadowEdgeRect(40,40,160,80);
	TXT(45,45,"ICBasic V. 0.4");
	TXT(45,58,"(C) 2002 Thomas Quester");
	TXT(45,75,"http://www.quester-hh/ic35");
	/* BTN("Init MMCard"); */
	strcpy(gTempBuf,"MMCard1");
    mInitialCard(gTempBuf);
	strcat(gTempBuf,"\\ICBasic");
	mOpenDirectory(gTempBuf,CREATE_ALWAYS,&iden);
	mCloseDirectory(&iden);
	/* BTN("InitVM"); */
	InitVM();
	/* BTN("InitParser"); */
	InitParser();
	/* BTN("InitInitMemory"); */
	ic_initMemory();

	BTN("Menu");

	LoadBasicFile();  
	DoMenu();

	
	
}