#include "Const.h"
#include <string>
#include <iostream>
#include "SymbolTable.h"
#include "Parser.h"
#include <sstream>
using namespace std;
int main()
{
	
	

	Parser* parser = new Parser("ksenija.txt");
	parser->parseFile();

	
	cout << intAsHex(0b01010101);
	return 0;
}