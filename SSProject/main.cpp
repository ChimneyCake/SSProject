#include "Const.h"
#include <string>
#include <iostream>
#include "SymbolTable.h"

using namespace std;
int main()
{
	for (int i = 0; i < Sections.size(); i++)
		cout << Sections.at(i) << endl;
	cout << isSection(".text");
	
	//cout << OperationCodes.at("JLZ");
	cout << RegisterCodes.at("R14")<<endl;
	//cout << AddressModeCodes.at(0b100);
	//cout << IsImmed("#123");
	//cout << IsRegInd("[R2]")<<endl;
	//cout << IsRegdir("R1")<<endl;

	cout << isRegindDisp("[R2 + 4]") << endl;
	string x = "Ksenija";
	toUpper(x);
	cout << x << endl;
	cout << isSection(".RODATA") << endl;
	cout << isOneOperandInstruction("INT 4");

	SymbolTable* sim = new SymbolTable("KSENIJA");
	SymbolList->push_back(sim);
	string xx= ((SymbolList->front())->getName());
	cout << SymbolList->front()->getName();

	string tmp = ".rodata";
	cout << tmp.substr(1, tmp.length() - 1) << endl;

	cout << convertStringToInt("0x20");
	cout << convertStringToInt("0b0101");
	cout << convertStringToInt("25");

	return 0;
}