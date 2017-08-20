#include "Content.h"

Content::Content(string name)
{
	this->hexCode = name;
}

Section* Content::getSection()
{
	return this->section;
}

void Content::setSection(Section* section)
{
	this->section = section;
}

string Content::getHexCode()
{
	return this->hexCode;
}

void Content::setHexCode(string code)
{
	this->hexCode = code;
}