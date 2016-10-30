#pragma once

#include "../../game_logic/event_system/IEventData.h"

#include "IController.h"
#include "ANDController.h"
#include "ORController.h"

#include <memory>
#include <map>

using std::map;
using std::pair;
using std::shared_ptr;

using namespace Game_Logic;

namespace Game_View
{
	class Sensor;

	/**
	 * A sensor has a listener map to watch the status of the 
     * keys, such as "MOUSE_BUTTON_1"
	 */
	typedef map<int, bool> Listener;

	typedef shared_ptr<Sensor> StrongSensorPtr;

	/**
	 * Base class for the sensors
	 */
	class Sensor
	{
	protected:
		/*
		 * Listener for the keys the sensor is watching (mouse and keyboard).
		 */
		Listener m_listener;
        
        /**
         * The sensor can listen to multiple keys and depending on the assigned
         * controller (ORController or ANDController), the sensor is active
         * if either one (ORController) or all (ANDController) has set its value in the
         * m_listener to active
         */
        StrongControllerPtr m_pController;

		/*
		 * Member variable that will be set true if the current sensor
         * has been initialized after first use
		 */
		bool m_initialized;

	public:
		/*
		 * Use an ORController by default
		 */
		Sensor()
		{
			SetController(new ORController());
		}
        
        virtual ~Sensor()
        {
        }
        
		virtual void VInit()
		{
			m_initialized = true;

			VOnInit();
		}

		/*
		 * Functionality that will be overwritten by the child classes
         * to specify behaviour that will be executed when the sensor
         * has initialized correctly
		 */
		virtual void VOnInit() = 0;

		/*
		 * Returns true if the current sensor is active
         * (see comments for m_pController for further details)
		 */
		virtual bool IsActive()
		{
			return m_pController->VIsActive(m_listener);
		}

		/*
		 * Returns true if a specific key is active at the moment
		 */
		virtual bool IsActiveKey(const int& key)
		{
			if (!AlreadyListeningTo(key))
			{
				return false;
			}

			return m_listener[key];
		}

		/*
		 * Adds a specific key to the listener map
		 */
		void AddListener(const int& key)
		{
			if (!m_initialized)
			{
				VInit();
			}

			if (AlreadyListeningTo(key))
			{
				return;
			}

			m_listener.insert(pair<int, bool>(key, false));
		}

		/*
		 * Verifies if the sensor is already listening to the given key
		 */
		bool AlreadyListeningTo(const int& key) const
		{
			if (m_listener.find(key) == m_listener.end())
			{
				return false;
			}

			return true;
		}

		/*
		 * Removes the given key from the listener map
		 */
		void RemoveListener(const int& key)
		{
			auto findIt = m_listener.find(key);

			if (findIt == m_listener.end())
			{
				return;
			}

			m_listener.erase(findIt);
		}

		/*
		 * Sets the behaviour of the current sensor or to be
         * more specific, how many values needs to be active to
         * activate the sensor
		 */
		void SetController(IController* pController)
		{
			m_pController = StrongControllerPtr(pController);
		}

		/*
		 * Sets the given key active. If the current sensor has an ORController
         * the sensor will be active
		 */
		void SetActiveValue(const int& key, const bool& value)
		{
			if (!AlreadyListeningTo(key))
			{
				return;
			}

			m_listener[key] = value;
		}
	};
}