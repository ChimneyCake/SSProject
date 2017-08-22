#ifndef Const_
#define Const_
#include <vector>
#include <string>
#include <map>
#include <regex>
#include <list>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include "SymbolTable.h"
#include <iostream>
#include "RelocationTable.h"
#include "Content.h"
#include <bitset>

using namespace std;
static vector<string> Sections = { ".RODATA", ".DATA", ".BSS", ".TEXT" };
static vector<string> Registers= { "R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15", "PC", "SP"};
static vector<string> SystemDefinedWords = { "DUP", "DEF", "ORG" };
static vector<string> Scope = { "local", "global" };
static vector<string> NoOperandInstructions = { "RET" };
static vector<string> OneOperandInstructions = {"INT","JMP", "CALL","PUSH", "POP"};
static vector<string> TwoOperandsInstructions = { "JZ", "JNZ", "JGZ", "JGEZ", "JLZ", "JLEZ", "LOAD", "STORE", "LOADUB", "LOADSB", "LOADUW", "LOADSW", "STOREB", "STOREW" };
static vector<string> ThreeOperandsInstructions = { "ADD", "SUB", "MUL", "DIV", "AND", "OR", "XOR",  "ASL", "ASR" };
static vector<string> JumpInstructions = { "JZ", "JNZ", "JGZ", "JGEZ", "JLZ", "JLEZ" };
static vector<string> ArithmeticInstructions = { "ADD", "SUB", "MUL", "DIV", "AND", "OR", "XOR", "NOT", "ASL", "ASR" };
static vector<string> LoadStoreInstructions = { "LOAD", "STORE", "LOADUB", "LOADSB", "LOADUW", "LOADSW", "STOREB", "STOREW" };
static vector<string> StackInstructions = { "PUSH", "POP" };
//static map<int, string> HexadecimalNumbers = { {0, "0x00"}, {1, "0x01"}, {2, "0x02"}, {3, "0x03"}, {4, "0x04"}, {5, "0x05"}, {6, "0x06"}, {7, "0x07"}, {8, "0x08"}, {9, "0x09"}, {10, "0x0A"}, {11, "0x0B"}, {12, "0x0C"}, {13, "0x0D"}, {14, "0x0E"}, {15, "0x0F"} };
static map<string, int> OperationCodes = { {"INT", 0x00}, {"JMP", 0x02}, {"CALL", 0x03}, {"RET", 0x01}, {"JZ", 0x04},{"JNZ", 0x05}, {"JGZ", 0x06}, {"JGEZ", 0x07}, {"JLZ", 0x08}, {"JLEZ", 0x09}, {"LOAD", 0x10},{ "LOADUB", 0x10 },{ "LOADSB", 0x10 },{ "LOADUW", 0x10 },{ "LOADSW", 0x10 },{"STORE", 0x11},{ "STOREB", 0x11 },{ "STOREW", 0x11 },{"PUSH", 0x20},{"POP", 0x21}, {"ADD", 0x30}, {"SUB", 0x31}, {"MUL", 0x32}, {"DIV", 0x33}, {"MOD", 0x34}, {"AND", 0x35}, {"OR", 0x36}, {"XOR", 0x37}, {"NOT", 0x38}, {"ASL", 0x39}, {"ASR", 0x3A} };
static map<string, int> RegisterCodes = { {"R0", 0x00}, {"R1", 0x01}, {"R2", 0x02}, {"R3", 0x03}, {"R4", 0x04}, {"R5", 0x05}, {"R6", 0x06}, {"R7", 0x07}, {"R8", 0x08}, {"R9", 0x09}, {"R10", 0x0A}, {"R11", 0x0B}, {"R12", 0x0C}, {"R13", 0x0D}, {"R14", 0x0E}, {"R15", 0x0F},{ "SP", 0x10 } ,{ "PC", 0x11 } };
static map<string, int> AddressModeCodes = { {"immed", 0b100}, {"regdir", 0b000}, {"memdir", 0b110}, {"regind", 0b010}, {"reginddisp", 0b111} };
//static map<string, string> RegCodes = { {"R0", "0x00"}, {"R1", "0x01"}, {"R2", "0x02"}, {"R3", "0x03"}, {"R4", "0x04"}, {"R5", "0x05"}, {"R6", "0x06"}, {"R7", "0x07"}, {"R8", "0x08"}, {"R9", "0x09"}, {"R10", "0x0A"}, {"R11", "0x0B"}, {"R12", "0x0C"}, {"R13", "0x0D"}, {"R14", "0x0E"}, {"R15", "0x0F"}, {"PC", "0x10"}, {"SP", "0x11"} };
//DW-double word
//WZ-word expanded with zeros
//WS-word expanded with sign
//BZ-byte expanded with zeros
//BS-byte expanded with sign
//static map<string, int> DataTypeCodes = { {"DW", 0b000}, {"WZ", 0b001}, {"WS", 0b101}, {"BZ", 0b011}, {"BS", 0b111} };

static list<SymbolTable*>* SymbolList = new list<SymbolTable*>();//list that has to be written is symbol table

static list<Section*>* OrgedSections = new list<Section*>();//list of all "ORG" sections, to check if there is overlaping

static list<Section*>* SectionList = new list<Section*>();//list of all sections

static list<Symbol*>* Symbols = new list<Symbol*>();//list od symbols that are not sections

static void toUpper(string &text)
{
	for (int i = 0; i < text.length(); i++)
		text[i] = toupper(text[i]);
}

static bool isSection(string text)
{
	toUpper(text);
	if (text.substr(0, 7) == ".RODATA") return true;
	if (text.substr(0, 5) == ".DATA") return true;
	if (text.substr(0, 5) == ".TEXT") return true;
	if (text.substr(0, 4) == ".BSS") return true;
	return false;
}

static bool isOrg(string text)
{
	toUpper(text);
	if (text.substr(0, 3) == "ORG")
		return true;
	else
		return false;
}

static bool isLabel(string text)
{
	string s = text.substr(0, text.find(" "));
	if (s[s.length()-1] == ':')
		return true;
	else 
		return false;
}

static bool isImmed(string opCode)
{
	if (opCode[0] == '#')
		return true;
	else
		return false;
}

static bool isPCRelative(string opCode)
{
	if (opCode[0] == '$')
		return true;
	else
		return false;
}

static bool isRegInd(string opCode)
{
	toUpper(opCode);
	regex regind("\\[R[[:digit:]][0-5]?\\]");
	return regex_match(opCode, regind);
}

static bool isRegdir(string opCode)
{
	toUpper(opCode);
	regex regdir("R[[:digit:]][0-5]?");
	return regex_match(opCode, regdir);
}

static bool isRegindDisp(string opCode)
{
	toUpper(opCode);
	if (opCode[0] == '[')
	{
		if (opCode[3]=='+'||opCode[4] == '+')
			if (opCode[opCode.length() - 1] == ']')
				return true;
	}
	return false;
}

static bool isMemdir(string opCode)
{
	if (!isImmed(opCode) && !isPCRelative(opCode) && !isRegdir(opCode) && !isRegInd(opCode) && !isRegindDisp(opCode))
		return true;
	else
		return false;
}

static bool isEnd(string text)
{
	toUpper(text);
	if (text == ".END")
		return true;
	else
		return false;
}

static bool isNoOperandInstruction(string text)
{
	toUpper(text);
	for (int i = 0; i < NoOperandInstructions.size(); i++)
		if (NoOperandInstructions.at(i) == text)
			return true;
	return false;
}

static bool isOneOperandInstruction(string text)
{
	toUpper(text);
	string firstWord = text.substr(0, text.find(" "));
	for (int i = 0; i < OneOperandInstructions.size(); i++)
		if (OneOperandInstructions.at(i) == firstWord)
			return true;
	return false;
}

static bool isTwoOPerandsInsttruction(string text)
{
	toUpper(text);
	string firstWord = text.substr(0, text.find(" "));
	for (int i = 0; i < TwoOperandsInstructions.size(); i++)
		if (TwoOperandsInstructions.at(i) == firstWord)
			return true;
	return false;
}

static bool isThreeOperandsInstruction(string text)
{
	toUpper(text);
	string firstWord = text.substr(0, text.find(" "));
	for (int i = 0; i < ThreeOperandsInstructions.size(); i++)
		if (ThreeOperandsInstructions.at(i) == firstWord)
			return true;
	return false;
}

static bool isStackInstruction(string text)
{
	toUpper(text);
	string firstWord = text.substr(0, text.find(" "));
	for (int i = 0; i < StackInstructions.size(); i++)
		if (StackInstructions.at(i) == firstWord)
			return true;
	return false;
}

static bool isHexadecimal(string text)
{
	if (text.substr(0, 2) == "0x")
		return true;
	else
		return false;
}

static bool isBinary(string text)
{
	if (text.substr(0, 2) == "0b")
		return true;
	else 
		return false;
}

static int convertStringToInt(string text)
{
	if (isHexadecimal(text))
	{
		string s = text.substr(2, text.length() - 2);
		char * p;
		int n = strtol(s.c_str(), &p, 16);
			return n;
	}
	else
		if (isBinary(text))
		{
			string a = text.substr(2, text.length() - 2);
			int n = 0;
			int mask = 1;
			for (int i = a.length() - 1; i >= 0; --i, mask <<= 1) {
				if (a.at(i) != '0') {
					n |= mask;
				}
			}
			return n;
		}
		else
			return atoi(text.c_str());
}

static string sectionName(string text)
{
	toUpper(text);
	if (text.substr(0, 7) == ".RODATA") text=".RODATA";
	if (text.substr(0, 5) == ".DATA") text = ".DATA";
	if (text.substr(0, 5) == ".TEXT") text=".TEXT";
	if (text.substr(0, 4) == ".BSS") text=".BSS";
	return text;
}

static bool isData(string text)
{
	toUpper(text);
	if (text.substr(0, 2) == "DB" || text.substr(0, 2) == "DW" || text.substr(0, 2) == "DD")
		return true;
	else
		return false;
}

static bool isDUP(string text)
{
	toUpper(text);
	string str = "DUP";
	if (text.find(str) != std::string::npos)
		return true;
	else
		return false;
}

static bool isDEF(string text)
{
	toUpper(text);
	string str = "DEF";
	if (text.find(str) != std::string::npos)
		return true;
	else
		return false;
}

static bool isArithmeticInstruction(string instruction)
{
	toUpper(instruction);
	for (int i = 0; i < ArithmeticInstructions.size(); i++)
		if (ArithmeticInstructions.at(i) == instruction)
			return true;
	return false;
}

static bool isLoadStoreInstruction(string instruction)
{
	toUpper(instruction);
	for (int i = 0; i < LoadStoreInstructions.size(); i++)
		if (LoadStoreInstructions.at(i) == instruction)
			return true;
	return false;
}

static bool isJumpInstruction(string instruction)
{
	toUpper(instruction);
	for (int i = 0; i < JumpInstructions.size(); i++)
		if (JumpInstructions.at(i) == instruction)
			return true;
	return false;
}

static SymbolTable* findById(unsigned int id)//finds SymbolTable object by id in SymbolList list
{
	list<SymbolTable*>::iterator it;
	SymbolTable* elem;
	for (it = SymbolList->begin(); it != SymbolList->end(); ++it)
	{
		if ((*it)->getId() == id)
			elem = *it;
	}
	return elem;
}

static bool isGlobal(string text)
{
	toUpper(text);
	if (text.substr(0, 7) == ".GLOBAL")
		return true;
	else
		return false;
}

static bool isInSymbols(string name)
{
	list<Symbol*>::iterator it;
	//Symbol* elem;
	for (it = Symbols->begin(); it != Symbols->end(); ++it)
	{
		if ((*it)->getName() == name)
			return true;
	}
	return false;
}

static Section* findSectionByName(string name)//find section by it's name
{
	list<Section*>::iterator it;
	for (it = SectionList->begin(); it != SectionList->end(); ++it)
	{
		if ((*it)->getName() == name)
			return *it;
	}
	return NULL;
}

static SymbolTable* findSymbolByName(string name)
{
	list<SymbolTable*>::iterator it;
	for (it = SymbolList->begin(); it !=SymbolList->end(); ++it)
	{
		if ((*it)->getName() == name)
			return *it;
	}
	return NULL;
}

static string intAsHex(int x)
{
	std::stringstream stream;
	stream << std::hex << x;
	std::string result(stream.str());
	string a;
	if (result.length() < 2)
	{
		int i = result.length();
		if (i == 0)
		{
			result.append("0");
			result.append("0");
		}
		else
		{
			string l = "";
			l.append("0");
			l += result[0];
			result = l;
		}
	}
	else {
		string l = "";
		l += result[0];
		l += result[1];
		result = l;
	}
	a.append(result);
	return a;
}

static string intDispAsHex(int x)
{
	std::stringstream stream;
	stream << std::hex << x;
	std::string result(stream.str());
	string a;
	int b = 8 - result.length();
	while (b > 0)
	{
		a += '0';
		b--;
	}
	a.append(result);
	return a;
}

static string intTwoBytesAsHex(int x)
{
	std::stringstream stream;
	stream << std::hex << x;
	std::string result(stream.str());
	string a;
	int b = 4 - result.length();
	while (b > 0)
	{
		a += '0';
		b--;
	}
	a.append(result);
	return a;
}

static string intRegAsBinary(int a)
{
	//int a = 1111165117;
	string binary("");
	int mask = 1;
	for (int i = 0; i < 5; i++)
	{
		if ((mask&a) >= 1)
			binary = "1" + binary;
		else
			binary = "0" + binary;
		mask <<= 1;
	}
	string ret = "";
	
	ret += binary[0];
	ret += binary[1];
	ret += binary[2];
	ret += binary[3];
	ret += binary[4];
	return ret;
}

static string intAddrModeAsBinary(int a)
{
	string binary("");
	int mask = 1;
	for (int i = 0; i < 3; i++)
	{
		if ((mask&a) >= 1)
			binary = "1" + binary;
		else
			binary = "0" + binary;
		mask <<= 1;
	}
	string ret = "";

	ret += binary[0];
	ret += binary[1];
	ret += binary[2];
	return ret;
}
static string intOpCodeAsBinary(int a)
{
	string binary("");
	int mask = 1;
	for (int i = 0; i < 8; i++)
	{
		if ((mask&a) >= 1)
			binary = "1" + binary;
		else
			binary = "0" + binary;
		mask <<= 1;
	}
	string ret = "";


	ret += binary[4];
	ret += binary[5];
	ret += binary[6];
	ret += binary[7];
	ret += binary[0];
	ret += binary[1];
	ret += binary[2];
	ret += binary[3];
	return ret;
}

static void setCountersToZero()
{
	list<Section*>::iterator it;
	for (it = SectionList->begin(); it != SectionList->end(); ++it)
	{
		(*it)->setLocationCounter(0);
	}
}

static bool checkOrgOverlaping()
{

}

static string returnHexCode(string code)
{
	string b4 = "0b";
	b4.append(code.substr(0, 8));
	b4 = intAsHex(convertStringToInt(b4));
	string tmp;
	tmp += b4[1];
	tmp += b4[0];
	b4 = tmp;

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

	return hexCode;
}

static bool isExpression(string text)
{
	if (std::string::npos != text.find_first_of("+-*/")) {
		return true;
	}
	return false;
}

static bool isConst(string text)
{
	return text.find_first_not_of("0123456789") == std::string::npos;
}

static string returnAsLittleEndian(string text)
{
	string tmp = "";

	tmp += text[6];
	tmp += text[7];

	tmp += text[4];
	tmp += text[5];

	tmp += text[2];
	tmp += text[3];

	tmp += text[0];
	tmp += text[1];

	return tmp;
}

static string returnAsHexString(int text)
{
	std::stringstream stream;
	stream << std::hex << text;
	std::string result(stream.str());
	string a;
	int b = 8 - result.length();
	a.append("0x");

	while (b > 0)
	{
		a.append("0");
		b--;
	}
	a.append(result);
	return a;
}
#endif // !Const_


