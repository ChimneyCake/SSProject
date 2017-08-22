#ifndef Content_
#define Content_
#include "SymbolTable.h"
#include <string>

using namespace std;

class Section;

class Content {
private:
	string instructionHexCode;
	string disp;
public:
	Content();
	~Content() {};

	string getInstructionHexCode();
	void setInstructionHexCode(string);

	string getDisp();
	void setDisp(string);
};

#endif // !Content_

