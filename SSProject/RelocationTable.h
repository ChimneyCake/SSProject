#ifndef RelocationTable_
#define RelocationTable_
#include "SymbolTable.h"
#include <string>

using namespace std;

class RelocationTable {
private:
	Section* section;
	int offset;
	string type;//absolut or relative
	unsigned int address;//hexadecimal number, represents address that has to be altered
	unsigned int id;//symbol id or section id when it goes about local symbool
public:
	RelocationTable();
	~RelocationTable() {};

	Section* getSection();
	void setSection(Section*);

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

