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
	checkOrgOverlaping();
	setCountersToZero();
	inputFile.clear();
	inputFile.seekg(0, ios::beg);
	while (!inputFile.eof())
	{
		getline(inputFile, line);
		if (line != "")
		{
			relocate(line);
		}
		else
			continue;
	}
	inputFile.close();
	writeInFile();
}

void Parser::relocate(string line)
{
	if (isSection(line))
	{
		tmpSection = findSectionByName(line);
		tmpSection->setLocationCounter(0);
	}
	else if (isLabel(line))
		relocateLabel(line);
	else if (isData(line) || isDEF(line))
		relocateData(line);
	else if (isGlobal(line))
		relocateGlobal(line);
	else relocateInstruction(line);

}

void Parser::relocateGlobal(string line)
{
	string label = line.substr(8, line.length() - 8);
	SymbolTable* sym = findSymbolByName(label);
	if (sym != NULL)
		sym->setScope("global");
	else
	{
		SymbolTable* sym = new Symbol(label);
		((Symbol*)sym)->setIdSection(0);
		sym->setIsSection(false);
		sym->setOffset(0);
		((Symbol*)sym)->setType("SYM");
		SymbolList->push_back(sym);
		Symbols->push_back((Symbol*)sym);
	}
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
	int num = 1;

	if (isDUP(line))
	{
		istringstream iss(line);
		string word;

		iss >> word;
		string type = word;

		iss >> word;
		string konst1 = word;
		iss >> word;
		iss >> word;
		string konst2 = word;

		num = calculateExpression(konst1);

		if (!isExpression(konst2) && !isConst(konst2))//tipa samo a
		{
			int offset;
			int id;
			int disp;
			string hexCode;
			SymbolTable* s = findSymbolByName(konst2);
			if (s != NULL)
			{
				//relokacija
				offset = tmpSection->getLocationCounter();

				if (s->getSection()->getOrgFlag() == true)
				{
					if (s->getScope() == "local")
					{
						id = tmpSection->getId();
						disp = 0;
					}
					else
					{
						id = s->getId();
						disp = 0;
					}
				}
				else
				{
					if (s->getScope() == "local")
					{
						id = tmpSection->getId();
						disp = s->getOffset();
					}
					else
					{
						id = s->getId();
						disp = 0;
					}
				}
				if (line.substr(0, 2) == "DB")
					hexCode = intAsHex(disp);
				if (line.substr(0, 2) == "DW")
					hexCode = intTwoBytesAsHex(disp);
				if (line.substr(0, 2) == "DD")
					hexCode = intDispAsHex(disp);
				int i = num;
				while (i > 0)
				{
					RelocationTable* reloc = new RelocationTable();
					reloc->setOffset(offset);
					if (line.substr(0, 2) == "DB")
						offset += 1;
					if (line.substr(0, 2) == "DW")
						offset += 2;
					if (line.substr(0, 2) == "DD")
						offset += 4;
					reloc->setId(id);
					reloc->setType("R");
					tmpSection->relocationTableList->push_back(reloc);
					Content* con = new Content();
					con->setInstructionHexCode(hexCode);
					tmpSection->contentList->push_back(con);
					i--;
				}
			}

		}
		else if ((isConst(konst2) && !isExpression(konst2)) || (isConst(konst2) && isExpression(konst2)) || konst2 == "?")//samo konst vrednost
		{
			string hexCode;
			if (konst2 != "?")
			{
				int value = calculateExpression(konst2);
				int i = num;
				while (i > 0)
				{
					if (line.substr(0, 2) == "DB")
						hexCode = intAsHex(value);
					if (line.substr(0, 2) == "DW")
						hexCode = intTwoBytesAsHex(value);
					if (line.substr(0, 2) == "DD")
						hexCode = intDispAsHex(value);
					i--;
				}
			}
			else
			{
				int i = num;
				while (i > 0)
				{
					if (line.substr(0, 2) == "DB")
						hexCode = "00";
					if (line.substr(0, 2) == "DW")
						hexCode = "0000";
					if (line.substr(0, 2) == "DD")
						hexCode = "00000000";
				}
			}

			int i = num;
			while (i > 0)
			{
				Content* con = new Content();
				con->setInstructionHexCode(hexCode);
				tmpSection->contentList->push_back(con);
				i--;
			}
		}
		else if (!isConst(konst2) && isExpression(konst2))//a+konst vrednost
		{
			string op;
			string exp;
			op = konst2.substr(0, konst2.find("+"));

			int i = 0;
			while (i < konst2.length() && konst2[i] != '+')
				i++;
			exp = konst2.substr(i + 1, konst2.length() - i - 1);

			int value = calculateExpression(exp);

			SymbolTable* s = findSymbolByName(op);
			if (s != NULL)
			{
				int offset;
				int id;
				string hexCode;
				int disp;
				if (s->getSection()->getOrgFlag() == true)
				{
					if (s->getScope() == "local")
					{
						id = tmpSection->getId();
						disp = 0 + value;
					}
					else
					{
						id = s->getId();
						disp = 0 + value;
					}
				}
				else
				{
					if (s->getScope() == "local")
					{
						id = tmpSection->getId();
						disp = s->getOffset() + value;
					}
					else
					{
						id = s->getId();
						disp = value;
					}
				}
				if (line.substr(0, 2) == "DB")
					hexCode = intAsHex(disp);
				if (line.substr(0, 2) == "DW")
					hexCode = intTwoBytesAsHex(disp);
				if (line.substr(0, 2) == "DD")
					hexCode = intDispAsHex(disp);
				int i = num;
				while (i > 0)
				{
					RelocationTable* reloc = new RelocationTable();
					reloc->setOffset(offset);
					if (line.substr(0, 2) == "DB")
						offset += 1;
					if (line.substr(0, 2) == "DW")
						offset += 2;
					if (line.substr(0, 2) == "DD")
						offset += 4;
					reloc->setId(id);
					reloc->setType("R");
					tmpSection->relocationTableList->push_back(reloc);
					Content* con = new Content();
					con->setInstructionHexCode(hexCode);
					tmpSection->contentList->push_back(con);
					i--;
				}
			}
		}
	}
	else if (isData(line))
	{
		int offset;
		int id;
		istringstream iss(line);
		string word;
		iss >> word;
		iss >> word;
		string sym = word;

		if (!isExpression(sym) && !isConst(sym))//tipa samo a
		{
			string hexCode;
			int disp;
			SymbolTable* s = findSymbolByName(sym);
			if (s != NULL)
			{
				//relokacija
				offset = tmpSection->getLocationCounter();

				if (s->getSection()->getOrgFlag() == true)
				{
					if (s->getScope() == "local")
					{
						id = tmpSection->getId();
						disp = 0;
					}
					else
					{
						id = s->getId();
						disp = 0;
					}
				}
				else
				{
					if (s->getScope() == "local")
					{
						id = tmpSection->getId();
						disp = s->getOffset();
					}
					else
					{
						id = s->getId();
						disp = 0;
					}
				}
				if (line.substr(0, 2) == "DB")
					hexCode = intAsHex(disp);
				if (line.substr(0, 2) == "DW")
					hexCode = intTwoBytesAsHex(disp);
				if (line.substr(0, 2) == "DD")
					hexCode = intDispAsHex(disp);
				RelocationTable* reloc = new RelocationTable();
				reloc->setOffset(offset);
				reloc->setId(id);
				reloc->setType("R");
				tmpSection->relocationTableList->push_back(reloc);
				Content* con = new Content();
				con->setInstructionHexCode(hexCode);
			}
		}
		else if ((isConst(sym) && !isExpression(sym)) || (isConst(sym) && isExpression(sym)) || sym == "?")//samo konst vrednost, nema relokacije
		{
			string hexCode;
			if (sym != "?")
			{
				int value = calculateExpression(sym);
				if (line.substr(0, 2) == "DB")
					hexCode = intAsHex(value);
				if (line.substr(0, 2) == "DW")
					hexCode = intTwoBytesAsHex(value);
				if (line.substr(0, 2) == "DD")
					hexCode = intDispAsHex(value);
			}
			else
			{
				if (line.substr(0, 2) == "DB")
					hexCode = "00";
				if (line.substr(0, 2) == "DW")
					hexCode = "0000";
				if (line.substr(0, 2) == "DD")
					hexCode = "00000000";
			}
			Content* con = new Content();
			con->setInstructionHexCode(hexCode);
			tmpSection->contentList->push_back(con);
		}
		else if (!isConst(sym) && isExpression(sym))//a+konst vrednost
		{
			string hexCode;
			string op;
			string exp;
			string type = "R";
			op = sym.substr(0, sym.find("+"));

			int i = 0;
			while (i < sym.length() && sym[i] != '+')
				i++;
			exp = sym.substr(i + 1, sym.length() - i - 1);

			int value = calculateExpression(exp);
			int disp;
			SymbolTable* s = findSymbolByName(op);
			if (s != NULL)
			{
				//relokacija
				offset = tmpSection->getLocationCounter();

				if (s->getSection()->getOrgFlag() == true)
				{
					if (s->getScope() == "local")
					{
						id = tmpSection->getId();
						disp = 0 + value;
					}
					else
					{
						id = s->getId();
						disp = 0 + value;
					}
				}
				else
				{
					if (s->getScope() == "local")
					{
						id = tmpSection->getId();
						disp = s->getOffset() + value;
					}
					else
					{
						id = s->getId();
						disp = value;
					}
				}
				if (line.substr(0, 2) == "DB")
					hexCode = intAsHex(disp);
				if (line.substr(0, 2) == "DW")
					hexCode = intTwoBytesAsHex(disp);
				if (line.substr(0, 2) == "DD")
					hexCode = intDispAsHex(disp);
				RelocationTable* reloc = new RelocationTable();
				reloc->setOffset(offset);
				reloc->setId(id);
				reloc->setType(type);
				tmpSection->relocationTableList->push_back(reloc);
				Content* con = new Content();
				con->setInstructionHexCode(hexCode);
				tmpSection->contentList->push_back(con);
			}

		}
	}

	if (line.substr(0, 2) == "DB")
		tmpSection->setLocationCounter((tmpSection->getLocationCounter() + 1)*num);
	if (line.substr(0, 2) == "DW")
		tmpSection->setLocationCounter((tmpSection->getLocationCounter() + 2)*num);
	if (line.substr(0, 2) == "DD")
		tmpSection->setLocationCounter((tmpSection->getLocationCounter() + 4)*num);


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

	string hexCode = returnHexCode(code);
	cout << hexCode << endl;

	Content* con = new Content();
	con->setInstructionHexCode(hexCode);
	((Section*)tmpSection)->contentList->push_back(con);
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

	string hexCode = returnHexCode(code);
	cout << hexCode << endl;

	Content* con = new Content();
	con->setInstructionHexCode(hexCode);
	((Section*)tmpSection)->contentList->push_back(con);

}

void Parser::contentNoRelocateTwoOperands(string line)
{
	istringstream iss(line);
	string word;
	iss >> word;

	string instruction = word;
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
		reg1 = word.substr(1, word.length() - 2);
	}

	reg1 = intRegAsBinary(RegisterCodes.at(reg1));

	string code = "";
	code.append(opCode);
	code.append(addrModeCode);
	code.append(reg0);
	code.append(reg1);

	if (isLoadStoreInstruction(instruction))
	{
		code.append("00000");
		if (instruction == "LOADUB")
			code.append("011");
		if (instruction == "LOADSB")
			code.append("111");
		if (instruction == "LOADUW")
			code.append("001");
		if (instruction == "LOADSW")
			code.append("101");
		if (instruction == "STOREB")
			code.append("011");
		if (instruction == "STOREW")
			code.append("001");
		if (instruction == "LOAD" || instruction == "STORE")
			code.append("000");
		code.append("000");
	}
	else
	{
		code.append("00000000000");
	}

	string hexCode = returnHexCode(code);

	cout << hexCode << endl;
	Content* con = new Content();
	con->setInstructionHexCode(hexCode);
	((Section*)tmpSection)->contentList->push_back(con);
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
		string addressMode = line.substr(position + 1, std::string::npos);
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
	if (isOneOperandInstruction(instruction))
	{
		istringstream iss(line);
		string word;

		iss >> word;//<=>instruction
		iss >> word;//operand

		string operand = word;
		if (isRegdir(operand) || isRegInd(operand))
		{
			tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 4);
			contentNoRelocateOneOperand(x);
		}
		else if (isImmed(operand) || isRegindDisp(operand) || isPCRelative(operand) || isMemdir(operand))
		{
			tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 8);
			contentRelocateOneOperand(x);
		}
	}
	if (isNoOperandInstruction(instruction))
	{
		string opCode = intOpCodeAsBinary(OperationCodes.at(instruction));
		string code = "";
		code.append(opCode);
		code.append("000000000000000000000000");

		string hexCode = returnHexCode(code);
	}
}

void Parser::contentNoRelocateOneOperand(string line)
{
	istringstream iss(line);

	string word;
	iss >> word;//instruction
	string instruction = word;
	string opCode = intOpCodeAsBinary(OperationCodes.at(word));

	string addressMode;
	iss >> word;
	string operand;
	if (isRegdir(word))
	{
		addressMode = "regdir";
		operand = word;
	}
	else
		if (isRegInd(word))
		{
			addressMode = "regind";
			operand = word.substr(1, word.length() - 2);
		}


	string addrModeCode = intAddrModeAsBinary(AddressModeCodes.at(addressMode));
	operand = intRegAsBinary(RegisterCodes.at(operand));

	string code;
	code.append(opCode);
	code.append(addrModeCode);
	code.append(operand);
	code.append("0000000000000000");

	string hexCode = returnHexCode(code);

	cout << hexCode;

	Content* con = new Content();
	con->setInstructionHexCode(hexCode);
	((Section*)tmpSection)->contentList->push_back(con);
}

void Parser::contentRelocateOneOperand(string line)
{
	istringstream iss(line);
	string word;
	iss >> word;
	string instruction = word;
	iss >> word;
	string second = word;
	string operand;
	string addressMode;
	string reg;//for reginddisp and pcrelative
	int disp;
	string type;
	int offset;
	int id;
	if (isPCRelative(second))
	{
		addressMode = "reginddisp";
		operand = second.substr(1, second.length() - 1);
		reg = "PC";
		type = "R";
		if (!isExpression(operand) && !isConst(operand))//ono sto vec imam-.txt fajl xD
		{
			SymbolTable* sym = findSymbolByName(operand);
			if (sym != NULL)
			{
				offset = tmpSection->getLocationCounter() - 4;
				if (sym->getSection()->getOrgFlag() == true)
				{
					if (tmpSection->getOrgFlag() == false)//PC nije ORG, a simbol jeste
					{
						id = 0;
						if (sym->getScope() == "local")
							disp = 0 - 4;
						else
							disp = 0;
					}
					RelocationTable* reloc = new RelocationTable();
					reloc->setType(type);
					reloc->setOffset(offset);
					reloc->setId(id);
					((Section*)tmpSection)->relocationTableList->push_back(reloc);
				}
				else
				{
					if (tmpSection->getOrgFlag() == true)//PC jeste iz ORG, simbol nije iz ORG
					{
						if (sym->getScope() == "local")
						{
							id = tmpSection->getId();
							disp = 0;
						}
						else
						{
							id = sym->getId();
							disp = 0 - 4;
						}
						RelocationTable* reloc = new RelocationTable();
						reloc->setType(type);
						reloc->setOffset(offset);
						reloc->setId(id);
						((Section*)tmpSection)->relocationTableList->push_back(reloc);
					}
					else//PC nije iz ORG, simbol nije iz ORG
					{
						if (sym->getScope() == "local")
						{
							id = tmpSection->getId();
							disp = sym->getOffset() - 4;
						}
						else
						{
							id = sym->getId();
							disp = 0 - 4;
						}
						RelocationTable* reloc = new RelocationTable();
						reloc->setType(type);
						reloc->setOffset(offset);
						reloc->setId(id);
						((Section*)tmpSection)->relocationTableList->push_back(reloc);
					}
				}
			}
		}
		else//else grana da se pozove funkcija za racunanje izraza
		{
			if (isConst(operand) && !isExpression(operand))
			{
				int res = calculateExpression(operand);
				offset = tmpSection->getLocationCounter() - 4;
				id = 0;
				disp = res - 4;
				RelocationTable* reloc = new RelocationTable();
				reloc->setType(type);
				reloc->setOffset(offset);
				reloc->setId(id);
				((Section*)tmpSection)->relocationTableList->push_back(reloc);
			}
			else if (!isConst(operand) && isExpression(operand))
			{
				char sign;
				string op;
				int i = 0;
				while (!(operand[i] == '+' || operand[i] == '-' || operand[i] == '*' || operand[i] == '/'))
				{
					op += operand[i];
					i++;
				}
				sign = operand[i];
				string exp = operand.substr(op.length(), operand.length() - op.length());
				int res = calculateExpression(exp);

				SymbolTable* sym = findSymbolByName(op);
				if (sym != NULL)
				{
					offset = tmpSection->getLocationCounter() - 4;
					if (sym->getSection()->getOrgFlag() == true)
					{
						if (tmpSection->getOrgFlag() == false)//PC nije ORG, a simbol jeste
						{
							id = 0;
							if (sym->getScope() == "local")
								disp = 0 - 4 + res;
							else
								disp = 0 + res;
						}
						RelocationTable* reloc = new RelocationTable();
						reloc->setType(type);
						reloc->setOffset(offset);
						reloc->setId(id);
						((Section*)tmpSection)->relocationTableList->push_back(reloc);
					}
					else
					{
						if (tmpSection->getOrgFlag() == true)//PC jeste iz ORG, simbol nije iz ORG
						{
							if (sym->getScope() == "local")
							{
								id = tmpSection->getId();
								disp = 0 + res;
							}
							else
							{
								id = sym->getId();
								disp = 0 - 4 + res;
							}
							RelocationTable* reloc = new RelocationTable();
							reloc->setType(type);
							reloc->setOffset(offset);
							reloc->setId(id);
							((Section*)tmpSection)->relocationTableList->push_back(reloc);
						}
						else//PC nije iz ORG, simbol nije iz ORG
						{
							if (sym->getScope() == "local")
							{
								id = tmpSection->getId();
								disp = sym->getOffset() - 4 + res;
							}
							else
							{
								id = sym->getId();
								disp = 0 - 4 + res;
							}
							RelocationTable* reloc = new RelocationTable();
							reloc->setType(type);
							reloc->setOffset(offset);
							reloc->setId(id);
							((Section*)tmpSection)->relocationTableList->push_back(reloc);
						}
					}
				}
			}

		}
		addressMode = intAddrModeAsBinary(AddressModeCodes.at(addressMode));
		instruction = intOpCodeAsBinary(OperationCodes.at(instruction));
		reg = intRegAsBinary(RegisterCodes.at(reg));
		string code = "";
		code.append(instruction);
		code.append(addressMode);
		code.append(reg);
		code.append("0000000000000000");
		string memorydisplacement = intDispAsHex(disp);
		string hexCode = returnHexCode(code);
		cout << memorydisplacement;
		cout << hexCode << endl;
		Content* con = new Content();
		con->setInstructionHexCode(hexCode);
		con->setDisp(memorydisplacement);
		((Section*)tmpSection)->contentList->push_back(con);
	}
	else//else grana kada nije pcrelativno
	{
		type = "A";

		if (isImmed(second))
		{
			operand = second.substr(1, operand.length() - 1);
			addressMode = "immed";
		}
		else if (isRegindDisp(second))
		{
			int i = 0;
			while (second[++i] != '+')
			{
				reg += second[i];
			}
			i++;
			while (second[i] != ']')
			{
				operand += second[i];
				i++;
			}
			addressMode = "reginddisp";
		}
		else if (isMemdir(second))
		{
			operand = second;
			addressMode = "memdir";
		}
		if (!isExpression(operand) && !isConst(operand))
		{
			SymbolTable* sym = findSymbolByName(operand);
			if (sym != NULL)
			{
				offset = tmpSection->getLocationCounter() - 8;
				if (sym->getSection()->getOrgFlag() == false)
				{
					if (sym->getScope() == "local")
					{
						id = tmpSection->getId();
						disp = sym->getOffset();
					}
					else
					{
						id = sym->getId();
						disp = 0;
					}
					if (addressMode == "reginddisp")
					{
						reg = intRegAsBinary(RegisterCodes.at(reg));
					}
					else
					{
						reg = "00000";
					}
					RelocationTable* reloc = new RelocationTable();
					reloc->setType(type);
					reloc->setOffset(offset);
					reloc->setId(id);
					((Section*)tmpSection)->relocationTableList->push_back(reloc);
				}
				else//nema relokacije ako je orgovana sekcija, ali mora da se popuni memorija
				{
					disp = 0;
					if (addressMode == "reginddisp")
					{
						reg = intRegAsBinary(RegisterCodes.at(reg));
					}
					else
					{
						reg = "00000";
					}
				}

			}
		}
		else if (isConst(operand) && !isExpression(operand))
		{
			int res = calculateExpression(operand);
			offset = tmpSection->getLocationCounter() - 8;
			id = 0;
			disp = res;
			RelocationTable* reloc = new RelocationTable();
			reloc->setType(type);
			reloc->setOffset(offset);
			reloc->setId(id);
			((Section*)tmpSection)->relocationTableList->push_back(reloc);
		}
		else if (!isConst(operand) && isExpression(operand))
		{
			char sign;
			string op;
			int i = 0;
			while (!(operand[i] == '+' || operand[i] == '-' || operand[i] == '*' || operand[i] == '/'))
			{
				op += operand[i];
				i++;
			}
			sign = operand[i];
			string exp = operand.substr(op.length(), operand.length() - op.length());
			int res = calculateExpression(exp);

			SymbolTable* sym = findSymbolByName(op);
			if (sym != NULL)
			{
				offset = tmpSection->getLocationCounter() - 8;
				if (sym->getSection()->getOrgFlag() == false)
				{
					if (sym->getScope() == "local")
					{
						id = tmpSection->getId();
						disp = sym->getOffset() + res;
					}
					else
					{
						id = sym->getId();
						disp = 0 + res;
					}
					if (addressMode == "reginddisp")
					{
						reg = intRegAsBinary(RegisterCodes.at(reg));
					}
					else
					{
						reg = "00000";
					}
					RelocationTable* reloc = new RelocationTable();
					reloc->setType(type);
					reloc->setOffset(offset);
					reloc->setId(id);
					((Section*)tmpSection)->relocationTableList->push_back(reloc);
				}
				else//nema relokacije, ali mora da se popuni memorija
				{
					disp = 0;
					if (addressMode == "reginddisp")
					{
						reg = intRegAsBinary(RegisterCodes.at(reg));
					}
					else
					{
						reg = "00000";
					}
				}
			}
		}
		addressMode = intAddrModeAsBinary(AddressModeCodes.at(addressMode));
		instruction = intOpCodeAsBinary(OperationCodes.at(instruction));
		reg = intRegAsBinary(RegisterCodes.at(reg));
		string code = "";
		code.append(instruction);
		code.append(addressMode);
		code.append(reg);
		code.append("0000000000000000");
		string memorydisplacement = intDispAsHex(disp);
		string hexCode = returnHexCode(code);
		Content* con = new Content();
		con->setInstructionHexCode(hexCode);
		con->setDisp(memorydisplacement);
		((Section*)tmpSection)->contentList->push_back(con);
	}
}

void Parser::contentRelocateTwoOperands(string line)
{
	istringstream iss(line);
	string word;
	iss >> word;
	string instruction = word;

	iss >> word;
	string second = word;//reg0

	iss >> word;
	string third = word;//reg1

	int disp;
	int offset;
	int id;
	string addressMode;
	string operand;
	string reg;//for reginddisp and pcrel
	string type;

	if (isPCRelative(third))
	{
		addressMode = "reginddisp";
		type = "R";
		operand = third.substr(1, third.length() - 1);
		reg = "PC";
		if (!isExpression(operand) && !isConst(operand))
		{
			SymbolTable* sym = findSymbolByName(operand);
			if (sym != NULL)
			{
				offset = tmpSection->getLocationCounter() - 4;
				if (sym->getSection()->getOrgFlag() == true)
				{
					if (tmpSection->getOrgFlag() == false)//PC nije ORG, a simbol jeste
					{
						id = 0;
						if (sym->getScope() == "local")
							disp = 0 - 4;
						else
							disp = 0;
					}
					RelocationTable* reloc = new RelocationTable();
					reloc->setType(type);
					reloc->setOffset(offset);
					reloc->setId(id);
					((Section*)tmpSection)->relocationTableList->push_back(reloc);

				}
				else
				{
					if (tmpSection->getOrgFlag() == true)//PC jeste iz ORG, simbol nije iz ORG
					{
						if (sym->getScope() == "local")
						{
							id = tmpSection->getId();
							disp = 0;
						}
						else
						{
							id = sym->getId();
							disp = 0 - 4;
						}
						RelocationTable* reloc = new RelocationTable();
						reloc->setType(type);
						reloc->setOffset(offset);
						reloc->setId(id);
						((Section*)tmpSection)->relocationTableList->push_back(reloc);
					}
					else//PC nije iz ORG, simbol nije iz ORG
					{
						if (sym->getScope() == "local")
						{
							id = tmpSection->getId();
							disp = sym->getOffset() - 4;
						}
						else
						{
							id = sym->getId();
							disp = 0 - 4;
						}
						RelocationTable* reloc = new RelocationTable();
						reloc->setType(type);
						reloc->setOffset(offset);
						reloc->setId(id);
						((Section*)tmpSection)->relocationTableList->push_back(reloc);
					}
				}
			}
		}
		else if (isConst(operand) && !isExpression(operand))
		{
			int res = calculateExpression(operand);
			offset = tmpSection->getLocationCounter() - 4;
			id = 0;
			disp = res - 4;
			RelocationTable* reloc = new RelocationTable();
			reloc->setType(type);
			reloc->setOffset(offset);
			reloc->setId(id);
			((Section*)tmpSection)->relocationTableList->push_back(reloc);
		}
		else if (!isConst(operand) && isExpression(operand))
		{
			char sign;
			string op;
			int i = 0;
			while (!(operand[i] == '+' || operand[i] == '-' || operand[i] == '*' || operand[i] == '/'))
			{
				op += operand[i];
				i++;
			}
			sign = operand[i];
			string exp = operand.substr(op.length(), operand.length() - op.length());
			int res = calculateExpression(exp);

			SymbolTable* sym = findSymbolByName(op);
			if (sym != NULL)
			{
				offset = tmpSection->getLocationCounter() - 4;
				if (sym->getSection()->getOrgFlag() == true)
				{
					if (tmpSection->getOrgFlag() == false)//PC nije ORG, a simbol jeste
					{
						id = 0;
						if (sym->getScope() == "local")
							disp = 0 - 4 + res;
						else
							disp = 0 + res;
					}
					RelocationTable* reloc = new RelocationTable();
					reloc->setType(type);
					reloc->setOffset(offset);
					reloc->setId(id);
					((Section*)tmpSection)->relocationTableList->push_back(reloc);
				}
				else
				{
					if (tmpSection->getOrgFlag() == true)//PC jeste iz ORG, simbol nije iz ORG
					{
						if (sym->getScope() == "local")
						{
							id = tmpSection->getId();
							disp = 0 + res;
						}
						else
						{
							id = sym->getId();
							disp = 0 - 4 + res;
						}
						RelocationTable* reloc = new RelocationTable();
						reloc->setType(type);
						reloc->setOffset(offset);
						reloc->setId(id);
						((Section*)tmpSection)->relocationTableList->push_back(reloc);
					}
					else//PC nije iz ORG, simbol nije iz ORG
					{
						if (sym->getScope() == "local")
						{
							id = tmpSection->getId();
							disp = sym->getOffset() - 4 + res;
						}
						else
						{
							id = sym->getId();
							disp = 0 - 4 + res;
						}
						RelocationTable* reloc = new RelocationTable();
						reloc->setType(type);
						reloc->setOffset(offset);
						reloc->setId(id);
						((Section*)tmpSection)->relocationTableList->push_back(reloc);
					}
				}
			}
		}

		addressMode = intAddrModeAsBinary(AddressModeCodes.at(addressMode));
		string opCode = intOpCodeAsBinary(OperationCodes.at(instruction));
		string reg0 = intRegAsBinary(RegisterCodes.at(second));
		string reg1 = intRegAsBinary(RegisterCodes.at(reg));
		string code = "";
		code.append(opCode);
		code.append(addressMode);
		code.append(reg0);
		code.append(reg1);
		if (isLoadStoreInstruction(instruction) == true)
		{
			code.append("00000");
			if (instruction == "LOADUB")
				code.append("011");
			if (instruction == "LOADSB")
				code.append("111");
			if (instruction == "LOADUW")
				code.append("001");
			if (instruction == "LOADSW")
				code.append("101");
			if (instruction == "STOREB")
				code.append("011");
			if (instruction == "STOREW")
				code.append("001");
			if (instruction == "LOAD" || instruction == "STORE")
				code.append("000");
			code.append("000");
		}
		else
		{
			code.append("00000000000");
		}
		string memorydisplacement = intDispAsHex(disp);
		string hexCode = returnHexCode(code);
		cout << memorydisplacement;
		cout << hexCode << endl;
		Content* con = new Content();
		con->setInstructionHexCode(hexCode);
		con->setDisp(memorydisplacement);
		((Section*)tmpSection)->contentList->push_back(con);
	}
	else//else grana za !=pcrel
	{
		type = "A";
		string reg0;
		string reg1;
		if (isImmed(third))
		{
			addressMode = "immed";
			operand = third.substr(1, third.length() - 1);
			cout << operand << endl;
		}

		else if (isRegindDisp(third))
		{
			addressMode = "reginddisp";
			int i = 0;
			while (third[++i] != '+')
			{
				reg += third[i];
			}
			i++;
			while (third[i] != ']')
			{
				operand += third[i];
				i++;
			}
		}
		else if (isMemdir(third))
		{
			addressMode = "memdir";
			operand = third;
			cout << operand << endl;
		}

		if (!isExpression(operand) && !isConst(operand))
		{
			SymbolTable* sym = findSymbolByName(operand);
			if (sym != NULL)
			{
				offset = tmpSection->getLocationCounter() - 8;
				if (sym->getSection()->getOrgFlag() == false)
				{
					if (sym->getScope() == "local")
					{
						id = ((Symbol*)sym)->getIdSection();
						disp = sym->getOffset();
					}
					else
					{
						id = sym->getId();
						disp = 0;
					}
					if (addressMode == "reginddisp")
					{
						reg1 = intRegAsBinary(RegisterCodes.at(reg));
					}
					else
					{
						reg1 = "00000";
					}
					RelocationTable* reloc = new RelocationTable();
					reloc->setType(type);
					reloc->setOffset(offset);
					reloc->setId(id);
					((Section*)tmpSection)->relocationTableList->push_back(reloc);
				}
				else
				{
					disp = 0;
					if (addressMode == "reginddisp")
					{
						reg1 = intRegAsBinary(RegisterCodes.at(reg));
					}
					else
					{
						reg1 = "00000";
					}
				}
			}
		}
		else if (isConst(operand) && !isExpression(operand))
		{
			int res = calculateExpression(operand);
			offset = tmpSection->getLocationCounter() - 8;
			id = 0;
			disp = res;
			reg1 = "00000";
			RelocationTable* reloc = new RelocationTable();
			reloc->setType(type);
			reloc->setOffset(offset);
			reloc->setId(id);
			((Section*)tmpSection)->relocationTableList->push_back(reloc);
		}
		else if (!isConst(operand) && isExpression(operand))
		{
			char sign;
			string op;
			int i = 0;
			while (!(operand[i] == '+' || operand[i] == '-' || operand[i] == '*' || operand[i] == '/'))
			{
				op += operand[i];
				i++;
			}
			sign = operand[i];
			string exp = operand.substr(op.length(), operand.length() - op.length());
			int res = calculateExpression(exp);

			SymbolTable* sym = findSymbolByName(op);

			if (sym != NULL)
			{
				offset = tmpSection->getLocationCounter() - 8;
				if (sym->getSection()->getOrgFlag() == false)
				{
					if (sym->getScope() == "local")
					{
						id = ((Symbol*)sym)->getIdSection();
						disp = sym->getOffset() + res;
					}
					else
					{
						id = sym->getId();
						disp = 0 + res;
					}
					if (addressMode == "reginddisp")
					{
						reg1 = intRegAsBinary(RegisterCodes.at(reg));
					}
					else
					{
						reg1 = "00000";
					}
					RelocationTable* reloc = new RelocationTable();
					reloc->setType(type);
					reloc->setOffset(offset);
					reloc->setId(id);
					((Section*)tmpSection)->relocationTableList->push_back(reloc);
				}
				else
				{
					disp = 0;
					if (addressMode == "reginddisp")
					{
						reg1 = intRegAsBinary(RegisterCodes.at(reg));
					}
					else
					{
						reg1 = "00000";
					}
				}
			}
		}
		reg0 = intRegAsBinary(RegisterCodes.at(second));
		string opCode = intOpCodeAsBinary(OperationCodes.at(instruction));
		addressMode = intAddrModeAsBinary(AddressModeCodes.at(addressMode));
		string code = "";
		code.append(opCode);
		code.append(addressMode);
		code.append(reg0);
		code.append(reg1);
		if (isLoadStoreInstruction(instruction) == true)
		{
			code.append("00000");
			if (instruction == "LOADUB")
				code.append("011");
			if (instruction == "LOADSB")
				code.append("111");
			if (instruction == "LOADUW")
				code.append("001");
			if (instruction == "LOADSW")
				code.append("101");
			if (instruction == "STOREB")
				code.append("011");
			if (instruction == "STOREW")
				code.append("001");
			if (instruction == "LOAD" || instruction == "STORE")
				code.append("000");
			code.append("000");
		}
		else
		{
			code.append("00000000000");
		}

		string memorydisplacement = intDispAsHex(disp);
		string hexCode = returnHexCode(code);
		cout << memorydisplacement;
		cout << hexCode << endl;

		Content* con = new Content();
		con->setInstructionHexCode(hexCode);
		con->setDisp(memorydisplacement);
		((Section*)tmpSection)->contentList->push_back(con);
	}
}

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
	else if (isData(line) || isDEF(line))
		data(line);
	else
		instruction(line);

}

void Parser::parseGlobal(string line)
{
	string label = line.substr(8, line.length() - 8);
	SymbolTable* sym = findSymbolByName(label);
	if (sym != NULL)
	{
		((Symbol*)sym)->setIdSection(-1);
	}
	else
		if (tmpSection != NULL)
			parseLabel(label);
}

void Parser::parseOrg(string line)
{
	string value = line.substr(4, line.length() - 4);
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
	if (previous != NULL && previous->getName() == "ORG")
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
	string s = line.substr(0, line.find(":"));
	size_t position = line.find(" ");
	string more = line.substr(position + 1, line.length() - s.length());//something has to be done with this

	if (isData(more))
		data(more);
	else
	{
		SymbolTable* sym = new Symbol(s);
		previous = current;
		current = sym;
		sym->setSection(tmpSection);
		((Symbol*)sym)->setType("SYM");
		if (sym->getSection()->getOrgFlag() == true)
			sym->setOffset(orgValue + sym->getSection()->getLocationCounter());
		else
			sym->setOffset(sym->getSection()->getLocationCounter());
		((Symbol*)sym)->setIdSection(tmpSection->getId());
		if (!isInSymbols(sym->getName()))
		{
			SymbolList->push_back(sym);
			Symbols->push_back((Symbol*)sym);
		}
		instruction(more);
	}
}

void Parser::data(string line)
{
	//string name = line.substr(0, line.find(":"));
	//size_t position = line.find(" ");
	//string more = line.substr(position + 1, line.length() - name.length());//something has to be done with this

	int num = 1;

	if (isDUP(line))
	{
		istringstream iss(line);
		string word;

		iss >> word;
		string type = word;

		iss >> word;
		string konst1 = word;
		iss >> word;
		iss >> word;
		string konst2 = word;

		num = calculateExpression(konst1);
		if (line.substr(0, 2) == "DB")
			tmpSection->setLocationCounter((tmpSection->getLocationCounter() + 1)*num);
		if (line.substr(0, 2) == "DW")
			tmpSection->setLocationCounter((tmpSection->getLocationCounter() + 2)*num);
		if (line.substr(0, 2) == "DD")
			tmpSection->setLocationCounter((tmpSection->getLocationCounter() + 4)*num);
	}
	else if (isData(line))
	{
		if (line.substr(0, 2) == "DB")
			tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 1);
		if (line.substr(0, 2) == "DW")
			tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 2);
		if (line.substr(0, 2) == "DD")
			tmpSection->setLocationCounter(tmpSection->getLocationCounter() + 4);

	}


	if (isDEF(line))
	{
		istringstream iss(line);
		string word;
		iss >> word;

		string symbol = word;
		iss >> word;
		iss >> word;
		string val = word;
		int value = calculateExpression(val);

		SymbolTable* sym = new Symbol(symbol);
		sym->setOffset(value);
		((Symbol*)sym)->setIdSection(-1);
		((Symbol*)sym)->setType("SYM");
		//sym->setIsSection(false);
		sym->setSection(tmpSection);
		SymbolList->push_back(sym);
	}
}

char* Parser::expressionToParse = new char[2];

int Parser::calculateExpression(string operand)
{
	/*char* ar = new char[2];
	char* val = ar;
	int i = 0;
	while (i < operand.length())
	{
		*ar++= operand[i];
		i++;
	}
	char x = *val;
	cout << x<<endl;
	ar = "";*/
	char* val = expressionToParse;
	int i = 0;
	while (i < operand.length())
	{
		*expressionToParse++ = operand[i];
		i++;
	}
	expressionToParse = val;
	int res = expression();
	return res;
}

char Parser::peek()
{
	return *expressionToParse;
}

char Parser::get()
{
	return *expressionToParse++;
}

int Parser::number()
{
	int result = get() - '0';
	while (peek() >= '0' && peek() <= '9')
	{
		result = 10 * result + get() - '0';
	}
	return result;
}

int Parser::factor()
{
	if (peek() >= '0' && peek() <= '9')
		return number();
	else if (peek() == '(')
	{
		get(); // '('
		int result = expression();
		get(); // ')'
		return result;
	}
	else if (peek() == '-')
	{
		get();
		return -factor();
	}
	return 0; // error

}

int Parser::term()
{
	int result = factor();
	while (peek() == '*' || peek() == '/')
		if (get() == '*')
			result *= factor();
		else
			result /= factor();
	return result;
}

int Parser::expression()
{
	int result = term();
	while (peek() == '+' || peek() == '-')
		if (get() == '+')
			result += term();
		else
			result -= term();
	return result;
}



