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
	string offset;
	string type;//absolut or relative
	unsigned int id;//symbol id or section id when it goes about local symbool
public:
	RelocationTable();
	~RelocationTable() {};

	string getOffset();
	void setOffset(int);

	string getType();
	void setType(string);

	unsigned int getId();
	void setId(unsigned int);
};

#endif // !RelocationTable_

