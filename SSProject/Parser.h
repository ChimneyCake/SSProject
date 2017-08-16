#ifndef Parser_
#define Parser_

#include <fstream>
#include <string>
#include "Const.h"
#include <iostream>
#include <iterator>
#include <sstream>
#include "RelocationTable.h"
#include "Content.h"

using namespace std;

class RelocationTable;
class Content;

class Parser {
private:
	string path;

	SymbolTable* current;
	SymbolTable* previous;
	int orgValue;
	bool orgFlag;
	Section* tmpSection;

public:

	Parser(string path);
	~Parser() {};
	void parseFile();
	void parse(string&);//first pass
	void writeInFile();
	void relocate(string);
	void fillSection(string);

	void relocateLabel(string);
	void relocateData(string);
	void relocateInstruction(string);
	void relocateGlobal(string);

	void contentArithmetic(string);
	void contentStack(string);
	void contentNoRelocateTwoOperands(string);
	void contentRelocateTwoOperands(string);

	void parseOrg(string);
	void parseSection(string);
	void parseLabel(string);
	void parseGlobal(string);

	void data(string);
	void instruction(string);

	void setTmpSection(Section*);
	Section* getTmpSection();

};

#endif // !
