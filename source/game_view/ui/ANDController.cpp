#include "ANDController.h"

using namespace Game_Logic;

bool ANDController::VIsActive(Listener& listener)
{
	for (auto pair : listener)
	{
		if (!pair.second)
		{
			return false;
		}
	}

	return true;
}