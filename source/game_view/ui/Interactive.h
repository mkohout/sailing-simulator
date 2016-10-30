#pragma once

#include "KeyMapping.h"
#include "Sensor.h"
#include <vector>
#include <map>
#include <glm/mat4x4.hpp>
#include <fastdelegate/FastDelegate.h>
#include "../../application_layer/resources/XMLResource.h"

using std::vector;
using std::map;

using namespace Application_Layer;

namespace Game_View
{
    
    typedef fastdelegate::FastDelegate0<> PlayerAction;
    typedef std::pair<string, PlayerAction> ActionPair;
    
    /**
     * Abstract class to inherit from if the
     * child class needs to interact with the user and have
     * specific actions that needs to be executed if
     * the user pressed particular keys
     */
    class Interactive
    {
    protected:
        
        /**
         * The sensors the player component is using
         * to listen to the user input
         */
        vector<StrongSensorPtr> m_sensors;
        
        /**
         * The mapping between keys (such as "arrowLeft") and actions
         * such as "steerLeft"
         */
        map<string, string> m_actionMapping;
        
        /**
         * The mapping between actions, such as "steerLeft" and the
         * actual functionality
         */
        map<string, PlayerAction> m_actions;
        
        /**
         * Initializes the key layout (the keys the player component will listen to)
         */
        void InitKeyLayout(StrongXMLElementPtr);
        
        /**
         * Creates the sensors based on the prior initialized key layout
         * and the given action.
         * Such as creating a keyboardsensor for "arrowLeft" executing the
         * action "steerLeft" if active
         */
        void InitSensorForAction(const string&, const string&);
        
        /**
         * Initializes the callback function for the actions, that
         * will be executed if the corresponding sensors are active
         */
        virtual void VInitActions() = 0;
        
        /**
         * Iterates through the sensors in the player component to check
         * if any sensor is active
         */
        void CheckForActiveSensors(IEventDataPtr);
        
        /**
         * Returns the action that will be executed if the given key
         * was pressed
         */
        const string GetActionForKey(const string&);
        
        /**
         * Executes the callback function belonging to the given
         * action
         */
        void PerformAction(const string&);
        
        
    public:
        virtual void VInit();
    };
}