#include "MortarSingleton.h"

MortarHandler* MortarHandler::s_instance = NULL;

MortarHandler::MortarHandler()
{
}

MortarHandler::~MortarHandler()
{
}

MortarHandler* MortarHandler::GetInstance()
{
	if (!s_instance)
	{
		s_instance = new MortarHandler;
	}
	return s_instance;
}