#pragma once

#include <memory>
#include <map>
#include <string>

using std::map;
using std::string;
using std::pair;

namespace Game_View
{
    typedef pair<string, int> KeyPair;
    
	/**
	  * Base class for controllers, to evaluate a list of sensors
	  */
    class KeyMapping
	{
    private:
        
        static bool s_initialized;
        
        static map<string, int> s_mapping;
        
        static map<int, string> s_reversed;
        
        /**
         * Initializes the mapping between GLFW keys and the names of the key (as a string)
         */
        static void InitMapping();
        
        /**
         * Reverse mapping to find the according string to the given GLFW key
         */
        static void InitReversedMapping();
        
	public:
        static string GetKeyStringForGLFW(const int&);
        
        static int GetGLFWKeyForString(const string&);
	};

}