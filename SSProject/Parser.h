#ifndef Parser_
#define Parser_

#include <fstream>
#include <string>
#include "Const.h"
#include <iostream>

using namespace std;

class Parser {
private:
	string path;
	ifstream* inputFile;
	//string section;
	SymbolTable* current;
	SymbolTable* previous;
	int orgValue;
	bool orgFlag;
	Section* tmpSection;

	int locationCounterText;
	int locationCounterData;
	int locationCounterRoData;
	int locationCounterBss;
public:
	Parser(string path);
	~Parser() {};
	void parseFile();
	void parse(string&);
	void writeInFile(ofstream);
	void write();//just for testing

	void parseOrg(string);
	void parseSection(string);
	void parseLabel(string);

};

#endif // !
