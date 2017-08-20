#include "RelocationTable.h"

RelocationTable::RelocationTable(int offset, string type, unsigned int id)
{
	this->offset = offset;
	this->type = type;
	this->id = id;
}

Section* RelocationTable::getSection()
{
	return this->section;
}

void RelocationTable::setSection(Section* section)
{
	this->section = section;
}

int RelocationTable::getOffset()
{
	return this->offset;
}

void RelocationTable::setOffset(int offset)
{
	this->offset = offset;
}

string RelocationTable::getType()
{
	return this->type;
}
void RelocationTable::setType(string type)
{
	this->type = type;
}

unsigned int RelocationTable::getId()
{
	return this->id;
}

void RelocationTable::setId(unsigned int id)
{
	this->id = id;
}

