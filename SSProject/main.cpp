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
	string text = "02f10000";
	cout << returnAsLittleEndian(text);
	int x = 0x02f1;
	cout << returnAsHexString(x);
	return 0;
}