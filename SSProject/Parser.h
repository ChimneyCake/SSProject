#ifndef Parser_
#define Parser_

#include <fstream>
#include <string>
#include "Const.h"
#include <iostream>
#include <iterator>

using namespace std;

class Parser {
private:
	string path;
	//ifstream inputFile;
	//string section;
	SymbolTable* current;
	SymbolTable* previous;
	int orgValue;
	bool orgFlag;
	Section* tmpSection;

public:
	Parser(string path);
	~Parser() {};
	void parseFile();
	void parse(string&);
	void writeInFile();

	void write();//just for testing

	void parseOrg(string);
	void parseSection(string);
	void parseLabel(string);

	void data(string);
	void instruction(string);

	void setTmpSection(Section*);
	Section* getTmpSection();

};

#endif // !
