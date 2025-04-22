
#include "ic35sdk.h"

char __err00[3] = "OK";
char __err01[18] = "Expected variable";
char __err02[12] = "Expected as";
char __err03[14] = "Expected type";
char __err04[29] = "Variable not defined";
char __err05[11] = "Expected =";
char __err06[17] = "Expteted newline";
char __err07[12] = "Expected To";
char __err08[23] = "Variables do not match";
char __err09[13] = "Syntax Error";
char __err10[4]  = "End";

char __errUnexp[17] = "Unexpected Error";
char __nl[2]        = "\n";
char *Errors[11] = 
{
	__err00,
	__err01,
	__err02,
	__err03,
	__err04,
	__err05,
	__err06,
	__err07,
	__err08,
	__err09,
	__err10
};
