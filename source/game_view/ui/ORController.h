#pragma once

#include "IController.h"

namespace Game_Logic
{
	/**
	 * Provides functionality like an OR gate
	 */
	class ORController : public IController
	{
	public:
		/*
		 * Returns true if one of the listeners is true
		 */
		virtual bool VIsActive(Listener&);
	};
}