#pragma once

#include <string>

using std::string;

namespace Application_Layer
{
	/**
	 * Base class for exceptions in the game engine
	 */
	class EngineException
	{
	protected:
		string m_message;

	public:
		EngineException(string message)
		{
			SetMessage(message);
		}
		void SetMessage(string message)
		{
			m_message = message;
		}

		string GetMessage() const
		{
			return m_message;
		}
	};
}