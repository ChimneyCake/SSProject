#include "Parser.h"

Parser::Parser(string path)
{
	this->path = path;
	//inputFile(path);
	//this->inputFile = new ifstream(path);
	//section = "";
}

void Parser::setTmpSection(Section* section)
{
	this->tmpSection = section;
}

Section* Parser::getTmpSection()
{
	return tmpSection;
}

void Parser::parseFile()
{
	ifstream inputFile(path);
	string line;
	while (!inputFile.eof())
	{
		getline(inputFile, line);
		if (line != "")
			parse(line);
		else
			continue;
	}
	inputFile.close();
	writeInFile();
	write();
}

void Parser::writeInFile()
{
	ofstream outputFile;
	outputFile.open("res.txt");
	list<SymbolTable*>::iterator it;
	for (it = SymbolList->begin(); it != SymbolList->end(); ++it)
	{
		cout << "NAME:";
		cout << (*it)->getName() << endl;
		outputFile.write(((*it)->getName()).c_str(), (*it)->getName().length());
		cout << "ORGED?";
		cout << (*it)->getSection()->getOrgFlag() << endl;
	}
	outputFile.close();
}

void Parser::parse(string& line)
{
	if (isOrg(line))
		parseOrg(line);
	else if (isSection(line))
		parseSection(line);
	else if (isLabel(line))
		parseLabel(line);
	else if (isData(line))
		data(line);
	else
		instruction(line);

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
	/*SymbolTable* sym = SymbolList->front();
	cout << "NAME:";
	cout << sym->getName() << endl;
	cout << "IS ORG?:";
	cout << ((Section*)sym)->getOrgFlag() << endl;
	sym->getSection()->setName(sectionName(sym->getSection()->getName()));
	//sectionName(sym->getSection()->getName());
	cout << "SECTION NAME:";
	cout << sym->getSection()->getName() << endl;
	cout << "SCOPE:";
	cout << sym->getScope() << endl;
	cout << "OFFSET:";
	cout << sym->getOffset() << endl;*/
	list<SymbolTable*>::iterator it;
	for ( it= SymbolList->begin(); it != SymbolList->end(); ++it)
	{
		cout << "NAME:";
		cout<<(*it)->getName()<<endl;
		cout << "ORGED?";
		cout << (*it)->getSection()->getOrgFlag()<<endl;
	}
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
	((Symbol*)sym)->setIdSection(tmpSection->getId());
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
	string instruction = line.substr(0, line.find(" "));

	if (isArithmeticInstruction(instruction))
		tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 8);

	if (isNoOperandInstruction(instruction))
		tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 4);

	if (isOneOperandInstruction(instruction))
	{
		size_t position = line.find(" ");
		string addressMode = line.substr(position + 1, line.length() - 1 - instruction.length());
		if (isRegdir(addressMode) || isRegInd(addressMode))
			tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 4);
		else
			if (isImmed(addressMode) || isMemdir(addressMode) || isRegindDisp(addressMode) || isPCRelative(addressMode))
			tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 8);
			
	}

	if (isTwoOPerandsInsttruction(instruction))
	{
		size_t position = line.find(" ");
		position = line.find(" ", position + 1);
		string addressMode = line.substr(position, std::string::npos);
		if (isRegdir(addressMode) || isRegInd(addressMode))
			tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 4);
		else
			if(isMemdir(addressMode) || isRegindDisp(addressMode) || isPCRelative(addressMode))
				tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 8);
	}

	
}


