SysObjs=\Ic35sdk\CCTools\Rom
TemplateLink=\Ic35sdk\CCTools\Template
IntermediateDir=.\Z80Debug
OutputDir=.\Z80_Out
OutputName=App.rom
OutputFile=$(OutputDir)\$(OutputName)

DefineMacro=
IncludeDir=\IC35SDK\INCLUDE\ 
Compiler=Lance
CompileOption= -I$(IncludeDir) -O$(IntermediateDir) -oe -B -T -A
Linker=Link80

LINK_OBJS= \
	$(IntermediateDir)\compiler.obj \
	$(IntermediateDir)\data.obj \
	$(IntermediateDir)\datetime.obj \
	$(IntermediateDir)\disass.obj \
	$(IntermediateDir)\helptok.obj \
	$(IntermediateDir)\iclibq.obj \
	$(IntermediateDir)\icmemory.obj \
	$(IntermediateDir)\icstdioi.obj \
	$(IntermediateDir)\icstdioq.obj \
	$(IntermediateDir)\imgdata.obj \
	$(IntermediateDir)\main.obj \
	$(IntermediateDir)\tokens.obj \
	$(IntermediateDir)\ui.obj \
	$(IntermediateDir)\uihook.obj \
	$(IntermediateDir)\uitool.obj \
	$(IntermediateDir)\utility.obj \
	$(IntermediateDir)\vm.obj \
	$(IntermediateDir)\vmapi.obj


DEF_FILE= \
	"App.lnk" \
	"App.sep" \
	"App.epp"

ALL :"AddFarcall""$(OutputDir)\App.rom"
"$(OutputDir)" :
	@if not exist "$(OutputDir)/$(NULL)" mkdir "$(OutputDir)"

!IF "$(OutputDir)" != "$(IntermediateDir)"
"$(IntermediateDir)":
	@if not exist "$(IntermediateDir)/$(NULL)" mkdir "$(IntermediateDir)"
!ENDIF

"AddFarcall" :
	-downcall app.afc /t
	@echo .

"$(OutputDir)\App.rom" : "$(OutputDir)" $(DEF_FILE) $(LINK_OBJS)
	@echo Step 1: Linking...
	@$(Linker) app

	@echo Step 2: Check if the memory in one bank is full?
	@check8 $(OutputDir)\app.map
	@echo !

	@echo Step 3: Seperate to Binary in each bank ...
	@sep00 $(OutputDir)\app.hex app.sep
	@echo !

	@echo Step 4: Merge to one ROM Code (app.rom) ...
	@file_epp app.epp $(OutputDir)\app.rom


SourceFile = $(SRCDIR)\$(SOURCE).$(SRCEXT)
ObjectFile = $(IntermediateDir)\$(SOURCE).obj

SRCDIR=.
SOURCE=main
SRCEXT=tmp
DEP_MAIN= \
	"\IC35SDK\INCLUDE\ic35sdk.h" \
	".\data.h" \
	".\vm.h" \
	".\uitool.h" \
	".\ui.h" \
	".\compiler.h" \
	".\icmemory.h" \
	".\icstdioq.h" \
	".\imgdata.h" \
	"\IC35SDK\INCLUDE\type.h" \
	"\IC35SDK\INCLUDE\key.h" \
	"\IC35SDK\INCLUDE\tcpip.h" \
	"\IC35SDK\INCLUDE\uart.h" \
	"\IC35SDK\INCLUDE\sc.h" \
	"\IC35SDK\INCLUDE\mmc.h"
"$(IntermediateDir)\main.obj" : $(SourceFile) $(DEP_MAIN) "$(IntermediateDir)"
	@if exist "$(IntermediateDir)\main.obj" erase "$(IntermediateDir)\main.obj"
	@echo $(SOURCE).c
	@$(Compiler) $(CompileOption) $(SourceFile)
	@del $(SourceFile)

SRCDIR=.
SOURCE=ui
SRCEXT=tmp
DEP_UI= \
	"\IC35SDK\INCLUDE\ic35sdk.h" \
	".\data.h" \
	".\compiler.h" \
	".\vm.h" \
	".\uitool.h" \
	".\icstdioq.h" \
	".\icmemory.h" \
	".\imgdata.h" \
	"\IC35SDK\INCLUDE\type.h" \
	"\IC35SDK\INCLUDE\key.h" \
	"\IC35SDK\INCLUDE\tcpip.h" \
	"\IC35SDK\INCLUDE\uart.h" \
	"\IC35SDK\INCLUDE\sc.h" \
	"\IC35SDK\INCLUDE\mmc.h"
"$(IntermediateDir)\ui.obj" : $(SourceFile) $(DEP_UI) "$(IntermediateDir)"
	@if exist "$(IntermediateDir)\ui.obj" erase "$(IntermediateDir)\ui.obj"
	@echo $(SOURCE).c
	@$(Compiler) $(CompileOption) $(SourceFile)
	@del $(SourceFile)

SRCDIR=.
SOURCE=compiler
SRCEXT=tmp
DEP_COMPI= \
	"\IC35SDK\INCLUDE\ic35sdk.h" \
	".\data.h" \
	".\compiler.h" \
	".\vm.h" \
	".\icmemory.h" \
	".\ui.h" \
	".\icstdioq.h" \
	".\compapi.h" \
	"\IC35SDK\INCLUDE\type.h" \
	"\IC35SDK\INCLUDE\key.h" \
	"\IC35SDK\INCLUDE\tcpip.h" \
	"\IC35SDK\INCLUDE\uart.h" \
	"\IC35SDK\INCLUDE\sc.h" \
	"\IC35SDK\INCLUDE\mmc.h"
"$(IntermediateDir)\compiler.obj" : $(SourceFile) $(DEP_COMPI) "$(IntermediateDir)"
	@if exist "$(IntermediateDir)\compiler.obj" erase "$(IntermediateDir)\compiler.obj"
	@echo $(SOURCE).c
	@$(Compiler) $(CompileOption) $(SourceFile)
	@del $(SourceFile)

SRCDIR=.
SOURCE=helptok
SRCEXT=tmp
DEP_HELPT= \
	"\IC35SDK\INCLUDE\ic35sdk.h" \
	".\data.h" \
	".\icstdioq.h" \
	".\vm.h" \
	"\IC35SDK\INCLUDE\type.h" \
	"\IC35SDK\INCLUDE\key.h" \
	"\IC35SDK\INCLUDE\tcpip.h" \
	"\IC35SDK\INCLUDE\uart.h" \
	"\IC35SDK\INCLUDE\sc.h" \
	"\IC35SDK\INCLUDE\mmc.h"
"$(IntermediateDir)\helptok.obj" : $(SourceFile) $(DEP_HELPT) "$(IntermediateDir)"
	@if exist "$(IntermediateDir)\helptok.obj" erase "$(IntermediateDir)\helptok.obj"
	@echo $(SOURCE).c
	@$(Compiler) $(CompileOption) $(SourceFile)
	@del $(SourceFile)

SRCDIR=.
SOURCE=tokens
SRCEXT=tmp
DEP_TOKEN= \
	"\IC35SDK\INCLUDE\ic35sdk.h" \
	".\data.h" \
	".\compiler.h" \
	".\compapi.h" \
	".\vm.h" \
	".\vmapi.h" \
	"\IC35SDK\INCLUDE\type.h" \
	"\IC35SDK\INCLUDE\key.h" \
	"\IC35SDK\INCLUDE\tcpip.h" \
	"\IC35SDK\INCLUDE\uart.h" \
	"\IC35SDK\INCLUDE\sc.h" \
	"\IC35SDK\INCLUDE\mmc.h" \
	".\icstdioq.h"
"$(IntermediateDir)\tokens.obj" : $(SourceFile) $(DEP_TOKEN) "$(IntermediateDir)"
	@if exist "$(IntermediateDir)\tokens.obj" erase "$(IntermediateDir)\tokens.obj"
	@echo $(SOURCE).c
	@$(Compiler) $(CompileOption) $(SourceFile)
	@del $(SourceFile)

SRCDIR=.
SOURCE=vm
SRCEXT=tmp
DEP_VM= \
	"\IC35SDK\INCLUDE\ic35sdk.h" \
	".\data.h" \
	".\vm.h" \
	".\vmapi.h" \
	".\icmemory.h" \
	".\icstdioq.h" \
	".\compiler.h" \
	".\utility.h" \
	".\imgdata.h" \
	".\ic35math.h" \
	"\IC35SDK\INCLUDE\type.h" \
	"\IC35SDK\INCLUDE\key.h" \
	"\IC35SDK\INCLUDE\tcpip.h" \
	"\IC35SDK\INCLUDE\uart.h" \
	"\IC35SDK\INCLUDE\sc.h" \
	"\IC35SDK\INCLUDE\mmc.h"
"$(IntermediateDir)\vm.obj" : $(SourceFile) $(DEP_VM) "$(IntermediateDir)"
	@if exist "$(IntermediateDir)\vm.obj" erase "$(IntermediateDir)\vm.obj"
	@echo $(SOURCE).c
	@$(Compiler) $(CompileOption) $(SourceFile)
	@del $(SourceFile)

SRCDIR=.
SOURCE=vmapi
SRCEXT=tmp
DEP_VMAPI= \
	"\IC35SDK\INCLUDE\ic35sdk.h" \
	".\data.h" \
	".\vm.h" \
	".\icmemory.h" \
	".\icstdioq.h" \
	".\compiler.h" \
	".\vmapi.h" \
	".\imgdata.h" \
	"\IC35SDK\INCLUDE\type.h" \
	"\IC35SDK\INCLUDE\key.h" \
	"\IC35SDK\INCLUDE\tcpip.h" \
	"\IC35SDK\INCLUDE\uart.h" \
	"\IC35SDK\INCLUDE\sc.h" \
	"\IC35SDK\INCLUDE\mmc.h"
"$(IntermediateDir)\vmapi.obj" : $(SourceFile) $(DEP_VMAPI) "$(IntermediateDir)"
	@if exist "$(IntermediateDir)\vmapi.obj" erase "$(IntermediateDir)\vmapi.obj"
	@echo $(SOURCE).c
	@$(Compiler) $(CompileOption) $(SourceFile)
	@del $(SourceFile)

SRCDIR=.
SOURCE=iclibq
SRCEXT=tmp
DEP_ICLIB= \
	"\IC35SDK\INCLUDE\ic35sdk.h" \
	".\data.h" \
	"\IC35SDK\INCLUDE\type.h" \
	"\IC35SDK\INCLUDE\key.h" \
	"\IC35SDK\INCLUDE\tcpip.h" \
	"\IC35SDK\INCLUDE\uart.h" \
	"\IC35SDK\INCLUDE\sc.h" \
	"\IC35SDK\INCLUDE\mmc.h" \
	".\icstdioq.h"
"$(IntermediateDir)\iclibq.obj" : $(SourceFile) $(DEP_ICLIB) "$(IntermediateDir)"
	@if exist "$(IntermediateDir)\iclibq.obj" erase "$(IntermediateDir)\iclibq.obj"
	@echo $(SOURCE).c
	@$(Compiler) $(CompileOption) $(SourceFile)
	@del $(SourceFile)

SRCDIR=.
SOURCE=utility
SRCEXT=tmp
DEP_UTILI= \
	".\utility.h" \
	"\IC35SDK\INCLUDE\ic35sdk.h" \
	"\IC35SDK\INCLUDE\type.h" \
	"\IC35SDK\INCLUDE\key.h" \
	"\IC35SDK\INCLUDE\tcpip.h" \
	"\IC35SDK\INCLUDE\uart.h" \
	"\IC35SDK\INCLUDE\sc.h" \
	"\IC35SDK\INCLUDE\mmc.h"
"$(IntermediateDir)\utility.obj" : $(SourceFile) $(DEP_UTILI) "$(IntermediateDir)"
	@if exist "$(IntermediateDir)\utility.obj" erase "$(IntermediateDir)\utility.obj"
	@echo $(SOURCE).c
	@$(Compiler) $(CompileOption) $(SourceFile)
	@del $(SourceFile)

SRCDIR=.
SOURCE=datetime
SRCEXT=tmp
DEP_DATET= \
	"\IC35SDK\INCLUDE\ic35sdk.h" \
	".\icstdioq.h" \
	"\IC35SDK\INCLUDE\type.h" \
	"\IC35SDK\INCLUDE\key.h" \
	"\IC35SDK\INCLUDE\tcpip.h" \
	"\IC35SDK\INCLUDE\uart.h" \
	"\IC35SDK\INCLUDE\sc.h" \
	"\IC35SDK\INCLUDE\mmc.h"
"$(IntermediateDir)\datetime.obj" : $(SourceFile) $(DEP_DATET) "$(IntermediateDir)"
	@if exist "$(IntermediateDir)\datetime.obj" erase "$(IntermediateDir)\datetime.obj"
	@echo $(SOURCE).c
	@$(Compiler) $(CompileOption) $(SourceFile)
	@del $(SourceFile)

SRCDIR=.
SOURCE=icmemory
SRCEXT=tmp
DEP_ICMEM= \
	"\IC35SDK\INCLUDE\ic35sdk.h" \
	".\data.h" \
	".\icmemory.h" \
	".\compiler.h" \
	"\IC35SDK\INCLUDE\type.h" \
	"\IC35SDK\INCLUDE\key.h" \
	"\IC35SDK\INCLUDE\tcpip.h" \
	"\IC35SDK\INCLUDE\uart.h" \
	"\IC35SDK\INCLUDE\sc.h" \
	"\IC35SDK\INCLUDE\mmc.h" \
	".\icstdioq.h"
"$(IntermediateDir)\icmemory.obj" : $(SourceFile) $(DEP_ICMEM) "$(IntermediateDir)"
	@if exist "$(IntermediateDir)\icmemory.obj" erase "$(IntermediateDir)\icmemory.obj"
	@echo $(SOURCE).c
	@$(Compiler) $(CompileOption) $(SourceFile)
	@del $(SourceFile)

SRCDIR=.
SOURCE=disass
SRCEXT=tmp
DEP_DISAS= \
	"\IC35SDK\INCLUDE\ic35sdk.h" \
	".\data.h" \
	".\vm.h" \
	".\icstdioq.h" \
	"\IC35SDK\INCLUDE\type.h" \
	"\IC35SDK\INCLUDE\key.h" \
	"\IC35SDK\INCLUDE\tcpip.h" \
	"\IC35SDK\INCLUDE\uart.h" \
	"\IC35SDK\INCLUDE\sc.h" \
	"\IC35SDK\INCLUDE\mmc.h"
"$(IntermediateDir)\disass.obj" : $(SourceFile) $(DEP_DISAS) "$(IntermediateDir)"
	@if exist "$(IntermediateDir)\disass.obj" erase "$(IntermediateDir)\disass.obj"
	@echo $(SOURCE).c
	@$(Compiler) $(CompileOption) $(SourceFile)
	@del $(SourceFile)

SRCDIR=.
SOURCE=icstdioi
SRCEXT=tmp
DEP_ICSTD= \
	"\IC35SDK\INCLUDE\ic35sdk.h" \
	".\data.h" \
	".\icstdioq.h" \
	"\IC35SDK\INCLUDE\type.h" \
	"\IC35SDK\INCLUDE\key.h" \
	"\IC35SDK\INCLUDE\tcpip.h" \
	"\IC35SDK\INCLUDE\uart.h" \
	"\IC35SDK\INCLUDE\sc.h" \
	"\IC35SDK\INCLUDE\mmc.h"
"$(IntermediateDir)\icstdioi.obj" : $(SourceFile) $(DEP_ICSTD) "$(IntermediateDir)"
	@if exist "$(IntermediateDir)\icstdioi.obj" erase "$(IntermediateDir)\icstdioi.obj"
	@echo $(SOURCE).c
	@$(Compiler) $(CompileOption) $(SourceFile)
	@del $(SourceFile)

SRCDIR=.
SOURCE=icstdioq
SRCEXT=tmp
DEP_ICSTD= \
	"\IC35SDK\INCLUDE\ic35sdk.h" \
	".\data.h" \
	"\IC35SDK\INCLUDE\type.h" \
	"\IC35SDK\INCLUDE\key.h" \
	"\IC35SDK\INCLUDE\tcpip.h" \
	"\IC35SDK\INCLUDE\uart.h" \
	"\IC35SDK\INCLUDE\sc.h" \
	"\IC35SDK\INCLUDE\mmc.h" \
	".\icstdioq.h"
"$(IntermediateDir)\icstdioq.obj" : $(SourceFile) $(DEP_ICSTD) "$(IntermediateDir)"
	@if exist "$(IntermediateDir)\icstdioq.obj" erase "$(IntermediateDir)\icstdioq.obj"
	@echo $(SOURCE).c
	@$(Compiler) $(CompileOption) $(SourceFile)
	@del $(SourceFile)

SRCDIR=.
SOURCE=uitool
SRCEXT=tmp
DEP_UITOO= \
	"\IC35SDK\INCLUDE\ic35sdk.h" \
	".\uitool.h" \
	".\data.h" \
	"\IC35SDK\INCLUDE\type.h" \
	"\IC35SDK\INCLUDE\key.h" \
	"\IC35SDK\INCLUDE\tcpip.h" \
	"\IC35SDK\INCLUDE\uart.h" \
	"\IC35SDK\INCLUDE\sc.h" \
	"\IC35SDK\INCLUDE\mmc.h" \
	".\icstdioq.h"
"$(IntermediateDir)\uitool.obj" : $(SourceFile) $(DEP_UITOO) "$(IntermediateDir)"
	@if exist "$(IntermediateDir)\uitool.obj" erase "$(IntermediateDir)\uitool.obj"
	@echo $(SOURCE).c
	@$(Compiler) $(CompileOption) $(SourceFile)
	@del $(SourceFile)

SRCDIR=.
SOURCE=uihook
SRCEXT=tmp
DEP_UIHOO= \
	"\IC35SDK\INCLUDE\ic35sdk.h" \
	".\data.h" \
	".\compiler.h" \
	".\uitool.h" \
	"\IC35SDK\INCLUDE\type.h" \
	"\IC35SDK\INCLUDE\key.h" \
	"\IC35SDK\INCLUDE\tcpip.h" \
	"\IC35SDK\INCLUDE\uart.h" \
	"\IC35SDK\INCLUDE\sc.h" \
	"\IC35SDK\INCLUDE\mmc.h" \
	".\icstdioq.h"
"$(IntermediateDir)\uihook.obj" : $(SourceFile) $(DEP_UIHOO) "$(IntermediateDir)"
	@if exist "$(IntermediateDir)\uihook.obj" erase "$(IntermediateDir)\uihook.obj"
	@echo $(SOURCE).c
	@$(Compiler) $(CompileOption) $(SourceFile)
	@del $(SourceFile)

SRCDIR=.
SOURCE=data
SRCEXT=c
DEP_DATA= \
	"\IC35SDK\INCLUDE\ic35sdk.h" \
	".\icstdioq.h" \
	".\data.h" \
	".\compiler.h" \
	"\IC35SDK\INCLUDE\type.h" \
	"\IC35SDK\INCLUDE\key.h" \
	"\IC35SDK\INCLUDE\tcpip.h" \
	"\IC35SDK\INCLUDE\uart.h" \
	"\IC35SDK\INCLUDE\sc.h" \
	"\IC35SDK\INCLUDE\mmc.h"
"$(IntermediateDir)\data.obj" : $(SourceFile) $(DEP_DATA) "$(IntermediateDir)"
	@if exist "$(IntermediateDir)\data.obj" erase "$(IntermediateDir)\data.obj"
	@echo $(SOURCE).c
	@$(Compiler) $(CompileOption) $(SourceFile)

SRCDIR=.
SOURCE=imgdata
SRCEXT=c
DEP_IMGDA=
"$(IntermediateDir)\imgdata.obj" : $(SourceFile) "$(IntermediateDir)"
	@if exist "$(IntermediateDir)\imgdata.obj" erase "$(IntermediateDir)\imgdata.obj"
	@echo $(SOURCE).c
	@$(Compiler) $(CompileOption) $(SourceFile)
