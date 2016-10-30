#pragma once

#include <memory>
#include <map>

using std::map;
using std::shared_ptr;

namespace Game_Logic
{
	/**
	 * Data structure for holding the values of keys and their
	 * state
	 */
	typedef map<int, bool> Listener;

	/**
	  * Base class for controllers, to evaluate a list of sensors
	  */
	class IController
	{
    protected:
        Listener m_listener;
        
        bool HasListener(int key)
        {
            auto findIt = m_listener.find(key);
            
            if(findIt==m_listener.end())
            {
                return false;
            }
            
            return true;
        }
        
	public:
        
		/*
		 * Returns true or false dependding on the specific controller
		 */
		virtual bool VIsActive(Listener&) = 0;
	};

	typedef shared_ptr<IController> StrongControllerPtr;
}