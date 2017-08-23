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
	string type;
	int id;
public:
	RelocationTable();
	~RelocationTable() {};

	string getOffset();
	void setOffset(int);

	string getType();
	void setType(string);

	int getId();
	void setId(int);
};

#endif // !RelocationTable_

