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
	/*parser->calculateExpression("5+4+8");
	parser->calculateExpression("7+8");
	parser->calculateExpression("23+17");
	parser->calculateExpression("-4+1");*/
	parser->parseFile();
	//cout << isRegindDisp("[R12+a]");
	/*string s = "aksa";
	if (std::string::npos != s.find_first_of("0123456789")) {
		std::cout << "digit(s)found!" << std::endl;
	}*/
	cout << intAsHex(-4);
	return 0;
}