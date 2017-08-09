#ifndef Parser_
#define Parser_

#include <fstream>
#include <string>
#include "Const.h"

using namespace std;

class Parser {
private:
	string path;
	ifstream* inputFile;
	string section;
	SymbolTable* current;
	SymbolTable* previous;
	int orgValue;
	bool orgFlag;

public:
	Parser(string path);
	~Parser() {};
	void parseFile();
	void parse(string&);
	void writeInFile(ofstream);

	void parseOrg(string);
	void parseSection(string);
	void parseLabel(string);
	

};

#endif // !
