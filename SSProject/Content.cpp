#include "Content.h"
#include "Const.h"
Content::Content()
{
	disp = "";
}

string Content::getInstructionHexCode()
{
	return this->instructionHexCode;
}

void Content::setInstructionHexCode(string code)
{
	code = returnAsLittleEndian(code);
	this->instructionHexCode = code;
}

string Content::getDisp()
{
	return disp;
}

void Content::setDisp(string disp)
{
	disp = returnAsLittleEndian(disp);
	this->disp = disp;
}