#include "Parser.h"

Parser::Parser(string path)
{
	this->path = path;
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
	inputFile.clear();
	inputFile.seekg(0, ios::beg);
	while (!inputFile.eof())
	{
		getline(inputFile, line);
		if (line != " ")
		{
			relocate(line);
			fillSection(line);
		}
		else
			continue;
	}
	inputFile.close();
	writeInFile();
}

void Parser::fillSection(string line)
{

}

void Parser::relocate(string line)
{
	if (isSection(line))
	{
		tmpSection = findSectionByName(line);
		cout << tmpSection->getName();
	}
}

void Parser::writeInFile()
{
	ofstream outputFile;
	outputFile.open("res.txt");
	list<SymbolTable*>::iterator it;
	outputFile.write("#TabelaSimbola", 14);
	outputFile.write("\n", 1);
	for (it = SymbolList->begin(); it != SymbolList->end(); ++it)
	{
		if ((*it)->getIsSection() == true)
			outputFile.write("SEG", 3);
		else
			outputFile.write("SYM", 3);
		outputFile.write(" ", 1);

		unsigned int id = ((*it)->getId());
		stringstream idss;
		idss << id;
		string ids = idss.str();

		outputFile.write(ids.c_str(), ids.length());
		outputFile.write(" ", 1);

		outputFile.write((*it)->getName().c_str(), (*it)->getName().length());
		outputFile.write(" ", 1);

		unsigned int idsec = ((*it)->getSection())->getId();
		stringstream idsecss;
		idsecss << idsec;
		string idsecs = idsecss.str();

		outputFile.write(idsecs.c_str(), idsecs.length());
		outputFile.write(" ", 1);

		int offset = (*it)->getOffset();
		stringstream offsetss;
		offsetss << offset;
		string offsets = offsetss.str();

		outputFile.write(offsets.c_str(), offsets.length());


		outputFile.write("\n", 1);
	}
	outputFile.close();
}

void Parser::parse(string& line)
{
	if (isOrg(line))
		parseOrg(line);
	else if (isGlobal(line))
		parseGlobal(line);
	else if (isSection(line))
		parseSection(line);
	else if (isLabel(line))
		parseLabel(line);
	else if (isData(line))
		data(line);
	else
		instruction(line);

}

void Parser::parseGlobal(string line)
{
	string label = line.substr(9, line.length() - 9);
	parseLabel(label);
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

	section->setIsSection(true);

	SymbolList->push_back(section);
	SectionList->push_back((Section*)section);

	if (((Section*)section)->getOrgFlag() == 1)
		OrgedSections->push_back((Section*)section);

}

void Parser::parseLabel(string line)
{
	string s= line.substr(0, line.find(":"));
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
	((Symbol*)sym)->setIdSection(tmpSection->getId());
	if(!isInSymbols(sym->getName()))
	{
		SymbolList->push_back(sym);
		Symbols->push_back((Symbol*)sym);
	}
	
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


