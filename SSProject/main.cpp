#include "Const.h"
#include <string>
#include <iostream>
#include "SymbolTable.h"
#include "Parser.h"

using namespace std;
int main()
{
	//for (int i = 0; i < Sections.size(); i++)
		//cout << Sections.at(i) << endl;
	//cout << isSection(".text");
	
	Parser* parser = new Parser("C:\\Users\\Ksenija\\Desktop\\ksenija.txt");
	parser->parseFile();

	//cout << OperationCodes.at("JLZ");
	//cout << RegisterCodes.at("R14")<<endl;
	//cout << AddressModeCodes.at(0b100);
	//cout << IsImmed("#123");
	//cout << IsRegInd("[R2]")<<endl;
	//cout << IsRegdir("R1")<<endl;

	//cout << isRegindDisp("[R2 + 4]") << endl;
	/*string x = "Ksenija";
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
	cout << convertStringToInt("25");*/
	
	/*parser->parseOrg("ORG 0x20");
	parser->parseSection(".text");
	parser->parseLabel("a:");
	parser->parseSection(".data");


	parser->write();
	cout << endl;
	cout << isLabel("Ksenija:")<<endl;

	parser->parseLabel("Ksenija:");

	Section* text=new Section(".text");
	parser->setTmpSection(text);
	parser->instruction("ADD R1 R2 #4");
	parser->instruction("Load R1 [R3]");
	parser->instruction("Store R1 $12");
	parser->instruction("JlEz R4 [R5+4]");
	int xx=parser->getTmpSection()->getLocationCounter();
	cout << xx << endl;*/
	return 0;
}