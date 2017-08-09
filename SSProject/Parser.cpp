#include "Parser.h"

Parser::Parser(string path)
{
	this->path = path;
	//this->inputFile = new ifstream(path);
	//section = "";
	locationCounterText = 0;
	locationCounterData = 0;
	locationCounterRoData = 0;
	locationCounterBss = 0;
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
	section->setType("SEG");

	SymbolList->push_back(section);
	
	/*SymbolTable* section = new Section(line);
	previous = current;
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
	string tmp = line.substr(0, line.length() - 1);
	SymbolTable* label = new SymbolTable(tmp);
	label->setSection(tmpSection);
	if (tmpSection->getOrgFlag == true)
		label->setOffset(orgValue);
	else
		label->setOffset(0);
	label->setType("SYM");
	SymbolList->push_back(label);
}
