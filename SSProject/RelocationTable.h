#ifndef RelocationTable_
#define RelocationTable_
#include <string>
#include "SymbolTable.h"

using namespace std;

class SymbolTable;
class Section;
class Symbol;

class RelocationTable {
private:
	int offset;
	string type;//absolut or relative
	unsigned int id;//symbol id or section id when it goes about local symbool
	Section* section;
public:
	RelocationTable(int, string, unsigned int);
	~RelocationTable() {};

	Section* getSection();
	void setSection(Section* section);

	int getOffset();
	void setOffset(int);

	string getType();
	void setType(string);

	unsigned int getAddress();
	void setAddress(unsigned int);

	unsigned int getId();
	void setId(unsigned int);
};

#endif // !RelocationTable_

