#include "RelocationTable.h"
#include "Const.h"

RelocationTable::RelocationTable()
{
	offset = "";
}

string RelocationTable::getOffset()
{
	return this->offset;
}

void RelocationTable::setOffset(int offset)
{
	string tmp = returnAsHexString(offset);
	this->offset = tmp;
}

string RelocationTable::getType()
{
	return this->type;
}
void RelocationTable::setType(string type)
{
	this->type = type;
}

int RelocationTable::getId()
{
	return this->id;
}

void RelocationTable::setId(int id)
{
	this->id = id;
}

