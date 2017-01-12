#ifndef MORTAR_SINGLETON
#include "../Mortar/Mortar.h"
#include <vector>

class MortarHandler
{
private:
	static MortarHandler* s_instance;
public:
	static MortarHandler* GetInstance();
	MortarHandler();
	~MortarHandler();
	std::vector<CMortar*> Mortars;
};

#endif