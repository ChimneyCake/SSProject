#include "Parser.h"

Parser::Parser(string path)
{
	this->path = path;
	//this->inputFile = new ifstream(path);
	//section = "";
}

void Parser::parseFile()
{
	string line;
	inputFile->open(path);
	while (!inputFile->eof())
	{
		getline(*inputFile, line);
		parse(line);
	}
	inputFile->close();
}

void Parser::parse(string& line)
{
	if (isOrg(line))
		parseOrg(line);
	if (isSection(line))
		parseSection(line);
	if (isLabel(line))
		parseLabel(line);
}

void Parser::parseOrg(string line)
{
	string value = line.substr(4, line.length()-4);
	orgValue = convertStringToInt(value);
	previous = current;
	current = new SymbolTable("ORG");
}

void Parser::parseSection(string line)
{
	SymbolTable* section = new Section(line);
	previous = current;
	current = section;
	tmpSection = (Section*)section;
	if (previous!=NULL && previous->getName()== "ORG")
	{
		section->setOffset(orgValue);
		((Section*)(section))->setOrgFlag(true);
	}
	else
	{
		section->setOffset(0);
		((Section*)(section))->setOrgFlag(false);
	}
	section->setSection((Section*)section);
	((Section*)section)->setType("SEG");

	SymbolList->push_back(section);
	
	/*SymbolTable* section = new Section(line);
	//previous = current;
	current = section;
	if(previous->getName=="ORG")
		section->setOffset(orgValue);
	else
		section->setOffset(0);

	section->setType("SEG");
	
	string tmp = line;
	toUpper(tmp);
	this->section = tmp.substr(1, tmp.length() - 1);//this->section is the name of section 
	//section->setSection(this->section);
	SymbolList->push_back(section);*/

}

void Parser::write()
{
	SymbolTable* sym = SymbolList->front();
	cout << "NAME:";
	cout << sym->getName() << endl;
	cout << "IS ORG?:";
	cout << ((Section*)sym)->getOrgFlag() << endl;
	sectionName(sym->getSection()->getName());
	cout << "SECTION NAME:";
	cout << sym->getSection()->getName() << endl;
	cout << "SCOPE:";
	cout << sym->getScope() << endl;
	cout << "OFFSET:";
	cout << sym->getOffset() << endl;
}

void Parser::parseLabel(string line)
{
	string s= line.substr(0, line.find(" "));
	size_t position = line.find(" ");
	string more = line.substr(position+1, line.length() - s.length());//something has to be done with this

	SymbolTable* sym = new Symbol(s);
	previous = current;
	current = sym;
	sym->setSection(tmpSection);
	((Symbol*)sym)->setType("SYM");
	if (sym->getSection()->getOrgFlag() == true)
		sym->setOffset(orgValue+sym->getSection()->getLocationCounter());
	else
		sym->setOffset(sym->getSection()->getLocationCounter());

	SymbolList->push_back(sym);

	if (isData(more))
		data(more);
	else
		instruction(more);
}

void Parser::data(string line)
{
	if (line.substr(0, 2) == "DB")
		tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 1);
	if (line.substr(0, 2) == "DW")
		tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 2);
	if (line.substr(0, 2) == "DD")
		tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 4);
}

void Parser::instruction(string line)
{
	if (isArithmeticInstruction(line))
		tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 8);


}


