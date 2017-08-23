#include "SymbolTable.h"
#include "Const.h"
int SymbolTable::statid = 0;

SymbolTable::SymbolTable(string name)
{
	this->name = name;
	this->scope = "local";
	this->isSection = false;
}

string SymbolTable::getName()
{
	return this->name;
}

void SymbolTable::setName(string name)
{
	this->name = name;
}

bool SymbolTable::getIsSection()
{
	return isSection;
}

void SymbolTable::setIsSection(bool isSection)
{
	this->isSection = isSection;
}


Section* SymbolTable::getSection()
{
	return this->section;
}

void SymbolTable::setSection(Section* section)
{
	this->section = section;
}

string SymbolTable::getOffset()
{
	return offset;
}

int SymbolTable::getOffsetInt()
{
	return offsetint;
}

void SymbolTable::setOffsetInt(int x)
{
	this->offsetint = x;
}

void SymbolTable::setOffset(int offset)
{
	offsetint = offset;
	string tmp = returnAsHexString(offset);
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
	orgFlag = false;
	type = "SEG";
	locationCounter = 0;
	setIsSection(1);
	this->relocationTableList = new list<RelocationTable*>();
	this->contentList = new list<Content*>();
}

Section::~Section()
{
	list<Content*>::iterator it;
	for (it = contentList->begin(); it != contentList->end(); ++it)
	{
		delete *it;
	}
	list<RelocationTable*>::iterator itr;
	for (itr = relocationTableList->begin(); itr != relocationTableList->end(); ++itr)
	{
		delete *itr;
	}
	delete contentList;
	delete relocationTableList;
}

unsigned int Section::getLocationCounter()
{
	return locationCounter;
}

void Section::setLocationCounter(unsigned int count)
{
	this->locationCounter = count;
}

bool Section::getOrgFlag()
{
	return orgFlag;
}

void Section::setOrgFlag(bool flag)
{
	orgFlag = flag;
}

string Section::getType()
{
	return type;
}

void Section::setType(string type)
{
	this->type = type;
}

Symbol::Symbol(string name) :SymbolTable(name)
{
	type = "SYM";
	setIsSection(0);
}

string Symbol::getType()
{
	return type;
}

void Symbol::setType(string type)
{
	this->type = type;
}

int Symbol::getIdSection()
{
	return idSection;
}

void Symbol::setIdSection(int idSec)
{
	this->idSection = idSec;
}

int Symbol::getValue()
{
	return value;
}

void Symbol::setValue(int val)
{
	this->value = value;
}