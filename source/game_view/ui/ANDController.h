#pragma once

#include "IController.h"

namespace Game_Logic
{
	/**
	 * Provides functionality like an AND gate
	 */
	class ANDController : public IController
	{
	public:
		/*
		 * Returns true, if all seconds of the given map are true
		 */
		virtual bool VIsActive(Listener&);
	};
}