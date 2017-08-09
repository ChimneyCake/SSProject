#include "SymbolTable.h"

int SymbolTable::statid = 0;

SymbolTable::SymbolTable(string name)
{
	this->name = name;
	this->scope = "local";
}

string SymbolTable::getName()
{
	return this->name;
}

void SymbolTable::setName(string name)
{
	this->name = name;
}

string SymbolTable::getType()
{
	return this->type;
}

void SymbolTable::setType(string type)
{
	this->type = type;
}

Section* SymbolTable::getSection()
{
	return this->section;
}

void SymbolTable::setSection(Section* section)
{
	this->section = section;
}

int SymbolTable::getOffset()
{
	return offset;
}

void SymbolTable::setOffset(int offset)
{
	this->offset = offset;
}

unsigned int SymbolTable::getId()
{
	return this->id;
}

void SymbolTable::setId(unsigned int id)
{
	this->id = id;
}

string SymbolTable::getScope()
{
	return this->scope;
}

void SymbolTable::setScope(string scope)
{
	this->scope = scope;
}

Section::Section(string name) :SymbolTable(name)
{
	orgFlag = 0;
}

bool Section::getOrgFlag()
{
	return orgFlag;
}

void Section::setOrgFlag(bool flag)
{
	orgFlag = flag;
}