#ifndef Content_
#define Content_
#include "SymbolTable.h"

using namespace std;

class Section;

class Content {
private:
	Section* section;
	string hexCode;
public:
	Content() {};
	~Content() {};

	Section* getSection();
	void setSection(Section*);

	string getHexCode();
	void setHexCode(string code);
};

#endif // !Content_

