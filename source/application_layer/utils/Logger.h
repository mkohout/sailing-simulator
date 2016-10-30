#pragma once

#include <string>

using std::string;

namespace Application_Layer
{
	/**
	 * Logging class for debugging
	 */
	class Logger
	{
	public:
        /**
         * Severity level of the log message
         */
		enum LOG_FLAG {
			INFO,
			WARNING,
			ERROR
		};
        
	public:
		Logger() = default;

        /**
         * Log a string
         */
		void Log(LOG_FLAG, const string&);
	};
}