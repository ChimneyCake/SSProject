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
	//parser->parseFile();
	string sym = "a+1234";
	int i = 0;
	while (i < sym.length() && sym[i] != '+')
		i++;
	string op = sym.substr(i + 1, sym.length() - i - 1);
	cout << op;
	return 0;
}