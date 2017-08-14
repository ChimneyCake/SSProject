#include "Parser.h"

Parser::Parser(string path)
{
	this->path = path;
	this->orgFlag = false;
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
	//inputFile.close();
	setCountersToZero();
	inputFile.clear();
	inputFile.seekg(0, ios::beg);
	//inputFile.open(path);
	while (!inputFile.eof())
	{
		getline(inputFile, line);
		if (line != "")
		{
			relocate(line);
			//fillSection(line);
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
		//cout << tmpSection->getName();
		tmpSection->setLocationCounter(0);
	}
	else if (isLabel(line))
		relocateLabel(line);
	else if (isData(line))
		relocateData(line);
	else if (isGlobal(line))
		relocateGlobal(line);
	else relocateInstruction(line);

}
void Parser::relocateGlobal(string line)
{
	string label = line.substr(8, line.length() - 8);
	SymbolTable* sym = findSymbolByName(label);
	sym->setScope("global");
}
void Parser::relocateLabel(string line)
{
	string s = line.substr(0, line.find(":"));
	size_t position = line.find(" ");
	string more = line.substr(position + 1, line.length() - s.length());

	if (!more.empty())
	{
		if (isData(more))
			relocateData(more);
		else
			relocateInstruction(more);
	}
}
void Parser::relocateData(string line)
{
	if (line.substr(0, 2) == "DB")
		tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 1);
	if (line.substr(0, 2) == "DW")
		tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 2);
	if (line.substr(0, 2) == "DD")
		tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 4);
}

void Parser::contentArithmetic(string line)
{
	string instruction = line.substr(0, line.find(" "));
	toUpper(instruction);
	string opCode = intOpCodeAsBinary(OperationCodes.at(instruction));
	string addrMode = intAddrModeAsBinary(AddressModeCodes.at("regdir"));

	istringstream iss(line);
	string word;
	iss >> word;
	iss >> word;
	string reg0 = word;
	iss >> word;
	string reg1 = word;
	
	int r0 = RegisterCodes.at(reg0);
	int r1 = RegisterCodes.at(reg1);
	string reg2;

	reg0 = intRegAsBinary(r0);
	reg1 = intRegAsBinary(r1);
	//intRegAsBinary(RegisterCodes.at(reg0));
	
	if (instruction != "NOT")
	{
		iss >> word;
		reg2 = word;
		int r2 = RegisterCodes.at(reg2);
		reg2 = intRegAsBinary(r2);
	}
	else
		reg2 = "00000";

	string code = "";
	code.append(opCode);
	code.append(addrMode);
	code.append(reg0);
	code.append(reg1);
	code.append(reg2);
	code.append("000000");
	
	string b4 = "0b";
	b4.append(code.substr(0, 8));
	b4 = intAsHex(convertStringToInt(b4));

	string b3 = "0b";
	b3.append(code.substr(8, 8));
	b3 = intAsHex(convertStringToInt(b3));

	string b2 = "0b";
	b2.append(code.substr(16, 8));
	b2 = intAsHex(convertStringToInt(b2));

	string b1 = "0b";
	b1.append(code.substr(24, 8));
	b1 = intAsHex(convertStringToInt(b1));

	string hexCode = "";
	hexCode.append(b4);
	hexCode.append(b3);
	hexCode.append(b2);
	hexCode.append(b1);
	//cout << hexCode<<endl;
}

void Parser::contentStack(string line)
{

	string instruction = line.substr(0, line.find(" "));
	toUpper(instruction);
	string opCode = intOpCodeAsBinary(OperationCodes.at(instruction));
	string addrMode = intAddrModeAsBinary(AddressModeCodes.at("regdir"));

	istringstream iss(line);
	string word;
	iss >> word;
	iss >> word;
	string reg = word;

	int r = RegisterCodes.at(reg);
	reg = intRegAsBinary(r);

	string code = "";
	code.append(opCode);
	code.append(addrMode);
	code.append(reg);
	code.append("0000000000000000");

	string b4 = "0b";
	b4.append(code.substr(0, 8));
	b4 = intAsHex(convertStringToInt(b4));

	string b3 = "0b";
	b3.append(code.substr(8, 8));
	b3 = intAsHex(convertStringToInt(b3));

	string b2 = "00";
	string b1 = "00";

	string hexCode = "";
	hexCode.append(b4);
	hexCode.append(b3);
	hexCode.append(b2);
	hexCode.append(b1);
	//cout << hexCode << endl;
}

void Parser::relocateInstruction(string line)
{
	string x = line;
	string instruction = line.substr(0, line.find(" "));
	if (isArithmeticInstruction(instruction))
	{
		tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 4);
		contentArithmetic(x);
	}
	if (isStackInstruction(x))
	{
		tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 4);
		contentStack(x);
	}
	if (isTwoOPerandsInsttruction(instruction))
	{
		size_t position = line.find(" ");
		position = line.find(" ", position + 1);
		string addressMode = line.substr(position+1, std::string::npos);
		if (isRegdir(addressMode) || isRegInd(addressMode))
		{
			tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 4);
			contentNoRelocateTwoOperands(x);
		}
		else
			if (isImmed(addressMode) || isMemdir(addressMode) || isRegindDisp(addressMode) || isPCRelative(addressMode))
			{
				tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 8);
				contentRelocateTwoOperands(x);
			}
	}
}

void Parser::contentNoRelocateTwoOperands(string line)
{
	istringstream iss(line);
	string word;
	iss >> word;

	string opCode = intOpCodeAsBinary(OperationCodes.at(word));

	iss >> word;

	string reg0 = intRegAsBinary(RegisterCodes.at(word));

	iss >> word;
	string addressMode;
	if (isRegdir(word))
		addressMode = "regdir";
	if (isRegInd(word))
		addressMode = "regind";
	string addrModeCode = intAddrModeAsBinary(AddressModeCodes.at(addressMode));

	string reg1 = "";

	if (addressMode == "regdir")
	{
		reg1 = word;
	}
	else
	{
		reg1 = word.substr(1, word.length()-2);
	}

	reg1= intRegAsBinary(RegisterCodes.at(reg1));

	string code = "";
	code.append(opCode);
	code.append(addrModeCode);
	code.append(reg0);
	code.append(reg1);

}
void Parser::contentRelocateTwoOperands(string line)
{}
void Parser::instruction(string line)
{
	string instruction = line.substr(0, line.find(" "));

	if (isArithmeticInstruction(instruction))
		tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 4);

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
			if (isImmed(addressMode) || isMemdir(addressMode) || isRegindDisp(addressMode) || isPCRelative(addressMode))
				tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 8);
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

		if ((*it)->getIsSection() == false)
			outputFile.write((*it)->getScope().c_str(), (*it)->getScope().length());

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
	string label = line.substr(8, line.length() - 8);
	if(tmpSection!=NULL)
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



