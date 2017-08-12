#ifndef SecondRound_
#define SecondRound_
#include <string>

using namespace std;

class SecondRound {
private:
	string path;
public:
	SecondRound(string);
	~SecondRound() {};

	void relocate(string&);//second pass
	void fillSection();

};

#endif