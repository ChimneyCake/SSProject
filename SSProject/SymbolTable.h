#ifndef SymbolTable_
#define SymbolTable_

#include <string>

using namespace std;

class Section;

class SymbolTable
{
private:
	static int statid;
	string name;
	string type;//SEG/SYM
	Section *section;//maybe it's better to have a class for section, not just string
	int offset;
	unsigned int id = statid++;
	string scope;//local or global

public:

	SymbolTable(string);
	~SymbolTable() {};

	string getName();
	void setName(string);

	string getType();
	void setType(string);

	int getOffset();
	void setOffset(int);

	unsigned int getId();
	void setId(unsigned int);

	string getScope();
	void setScope(string);

	Section* getSection();
	void setSection(Section*);
};

class Section :public SymbolTable {
private:
	bool orgFlag;
public:
	Section(string);
	~Section(){}
	bool getOrgFlag();
	void setOrgFlag(bool);
};

#endif
