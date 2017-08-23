#ifndef SymbolTable_
#define SymbolTable_

#include <string>
#include "RelocationTable.h"
#include "Content.h"
#include <list>

using namespace std;

class Content;
class RelocationTable;
class Section;
class Symbol;

class SymbolTable
{
private:
	static int statid;
	string name;
	Section *section;//maybe it's better to have a class for section, not just string
	string offset;
	int offsetint;
	unsigned int id = statid++;
	string scope;//local or global
	bool isSection;
public:

	SymbolTable(string);
	~SymbolTable() {
		delete section;
	};

	string getName();
	void setName(string);

	int getOffsetInt();

	string getOffset();
	void setOffset(int);

	unsigned int getId();
	void setId(unsigned int);

	string getScope();
	void setScope(string);

	Section* getSection();
	void setSection(Section*);

	bool getIsSection();//1 for section, 0 for symbol
	void setIsSection(bool);
};

class Section :public SymbolTable {
private:
	bool orgFlag;
	string type;
	unsigned int locationCounter;

public:
	Section(string);
	~Section();
	bool getOrgFlag();
	void setOrgFlag(bool);
	string getType();
	void setType(string type);
	unsigned int getLocationCounter();
	void setLocationCounter(unsigned int);

	list<RelocationTable*>* relocationTableList;
	list<Content*>* contentList;
};

class Symbol :public SymbolTable {
private:
	string type;
	int idSection;
	int value;
public:
	Symbol(string);
	~Symbol() {};
	string getType();
	void setType(string);
	int getIdSection();
	void setIdSection(int);
	int getValue();
	void setValue(int);
};

#endif
