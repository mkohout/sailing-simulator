#include "ORController.h"

using namespace Game_Logic;

bool ORController::VIsActive(Listener& listener)
{
	for (auto pair : listener)
	{
		if (pair.second)
		{
			return true;
		}
	}

	return false;
}