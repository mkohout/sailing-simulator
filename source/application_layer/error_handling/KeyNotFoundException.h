#pragma once

#include "EngineException.h"

namespace Application_Layer
{
	/**
	 * Exception is thrown if a setting is requested that
	 * is not available in the current ressource
	 */
	class KeyNotFoundException : public EngineException
	{
	private:

	public:
		KeyNotFoundException(string message) : EngineException(message)
		{

		}
	};
}