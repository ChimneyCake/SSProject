#include "Const.h"
#include <string>
#include <iostream>
#include "SymbolTable.h"
#include "Parser.h"
#include <sstream>
using namespace std;
int main()
{
	
	Parser* parser = new Parser("C:\\Users\\Ksenija\\Documents\\Visual Studio 2015\\Projects\\SSProject\\SSProject\\ksenija.txt");
	parser->parseFile();
	//cout << isRegindDisp("[R12+a]");
	return 0;
}