#include "Parser.h"

Parser::Parser(string path)
{
	this->path = path;
	//this->inputFile = new ifstream(path);
	section = "";
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
}

void Parser::parseSection(string line)
{
	SymbolTable* section = new SymbolTable(line);
	previous = current;
	current = section;
	section->setType("SEG");
	section->setOffset(0);
	string tmp = line;
	toUpper(tmp);
	this->section = tmp.substr(1, tmp.length() - 1);
	//section->setSection(this->section);
	SymbolList->push_back(section);
}

void Parser::parseLabel(string line)
{
	string tmp = line.substr(0, line.length() - 1);
	SymbolTable* label = new SymbolTable(tmp);
	//label->setSection(this->section);
	label->setType("SYM");
	SymbolList->push_back(label);
}
