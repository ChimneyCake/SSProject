#ifndef Const_
#define Const_
#include <vector>
#include <string>
#include <map>
#include <regex>
#include <list>
#include <cstdlib>
#include "SymbolTable.h"

using namespace std;

static vector<string> Sections = { ".RODATA", ".DATA", ".BSS", ".TEXT" };
static vector<string> Registers= { "R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15", "PC", "SP"};
static vector<string> SystemDefinedWords = { "DUP", "DEF", "ORG" };
static vector<string> Scope = { "local", "global" };
static vector<string> NoOperandInstructions = { "RET" };
static vector<string> OneOperandInstructions = {"INT","JMP", "CALL","PUSH", "POP"};
static vector<string> TwoOperandsInstructions = { "JZ", "JNZ", "JGZ", "JGEZ", "JLZ", "JLEZ", "LOAD", "STORE" };
static vector<string> ThreeOperandsInstructions = { "ADD", "SUB", "MUL", "DIV", "AND", "OR", "XOR", "NOT", "ASL", "ASR" };
static map<string, int> OperationCodes = { {"INT", 0x00}, {"JMP", 0x02}, {"CALL", 0x03}, {"RET", 0x01}, {"JZ", 0x04},{"JNZ", 0x05}, {"JGZ", 0x06}, {"JGEZ", 0x07}, {"JLZ", 0x08}, {"JLEZ", 0x09}, {"LOAD", 0x10}, {"STORE", 0x11},{"PUSH", 0x20},{"POP", 0x21}, {"ADD", 0x20}, {"SUB", 0x20}, {"MUL", 0x21}, {"DIV", 0x22}, {"MOD", 0x23}, {"AND", 0x24}, {"OR", 0x25}, {"XOR", 0x24}, {"NOT", 0x25}, {"ASL", 0x26}, {"ASR", 0x26} };
static map<string, int> RegisterCodes = { {"R0", 0x00}, {"R1", 0x01}, {"R2", 0x02}, {"R3", 0x03}, {"R4", 0x04}, {"R5", 0x05}, {"R6", 0x06}, {"R7", 0x07}, {"R8", 0x08}, {"R9", 0x09}, {"R10", 0x0A}, {"R11", 0x0B}, {"R12", 0x0C}, {"R13", 0x0D}, {"R14", 0x0E}, {"R15", 0x0F} };
static map<string, int> AddressModeCodes = { {"immed", 0b100}, {"regdir", 0b000}, {"memdir", 0b110}, {"regind", 0b010}, {"reginddisp", 0b111} };
//DW-double word
//WZ-word expanded with zeros
//WS-word expanded with sign
//BZ-byte expanded with zeros
//BS-byte expanded with sign
static map<string, int> DataTypeCodes = { {"DW", 0b000}, {"WZ", 0b001}, {"WS", 0b101}, {"BZ", 0b011}, {"BS", 0b111} };
static list<SymbolTable*>* SymbolList = new list<SymbolTable*>();

static string removeSpace(string text)
{
	text.erase(remove_if(text.begin(), text.end(), isspace), text.end());
	return text;
}

static void toUpper(string &text)
{
	for (int i = 0; i < text.length(); i++)
		text[i] = toupper(text[i]);
}

static bool isSection(string text)
{
	/*for (int i = 0; i < text.length(); i++)
		text[i] = toupper(text[i]);
	for (int i = 0; i < Sections.size(); i++)
	{
		if (text == Sections.at(i))
			return true;
	}
	return false;*/
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
	if (text.substr(0, 4) == ".ORG")
		return true;
	else
		return false;
}

static bool isLabel(string text)
{
	if (text[text.length()] == ':')
		return true;
	else false;
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
	opCode = removeSpace(opCode);
	regex regind("\\[R[[:digit:]][0-5]?\\]");
	return regex_match(opCode, regind);
}

static bool isRegdir(string opCode)
{
	opCode = removeSpace(opCode);
	regex regdir("R[[:digit:]][0-5]?");
	return regex_match(opCode, regdir);
}

static bool isRegindDisp(string opCode)
{
	opCode = removeSpace(opCode);
	regex reginddisp("\\[R[[:digit:]][0-5]?\\+[[:digit:]]+\\]");
	return regex_match(opCode, reginddisp);
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

static void sectionName(string &text)
{
	toUpper(text);
	if (text.substr(0, 7) == ".RODATA") text=".RODATA";
	if (text.substr(0, 5) == ".DATA") text = ".DATA";
	if (text.substr(0, 5) == ".TEXT") text=".TEXT";
	if (text.substr(0, 4) == ".BSS") text=".BSS";
}
#endif // !Const_
