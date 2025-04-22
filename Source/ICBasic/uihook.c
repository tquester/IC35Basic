#include "ic35sdk.h"
#include "data.h"
#include "compiler.h"
#include "uitool.h"

U8 OnEditMenu(ICEditor *editor);

U8 CallOnEditMenu(ICEditor *editor)
{
	OnEditMenu(editor);
}

void CallEditor(ICEditor *editor, U8 *buffer, U16 length, U16 line)
{
	editor->OnMenu = CallOnEditMenu;
	ICEditFile(editor,buffer,length,line);
}