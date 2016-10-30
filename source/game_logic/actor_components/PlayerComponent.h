#pragma once

#include "ActorComponent.h"
#include "../actor_system/Actor.h"
#include "../../game_view/ui/KeyMapping.h"
#include "../../game_view/ui/Sensor.h"
#include "../../game_view/ui/Interactive.h"
#include <vector>
#include <map>
#include <glm/mat4x4.hpp>
#include <fastdelegate/FastDelegate.h>

using namespace Game_View;

namespace Game_Logic
{
    /**
     * ActorComponent that takes care of evaluating user input
     * and trigger events to transform the actor the player component
     * is assigned to
     */
    class PlayerComponent : public ActorComponent, public Interactive
    {
    private:
        /**
         * The speed the actor is moving forward
         */
        float m_speed;
        
        /**
         * The speed the actor is rotating,
         * here the torque is meant
         */
        float m_rotationalSpeed;
        
        /**
         * Method that will rotate the boat slightly to the left
         */
        void SteerLeft();
        
        /**
         * Method that will rotate the boat slightly to the right
         */
        void SteerRight();
        
        /**
         * Increases the wind by adding 1 m/s to the winds velocity
         */
        void IncreaseWind();
        
        /**
         * Increases the wind by subtracting 1 m/s from the winds velocity
         */
        void DecreaseWind();
        
        /**
         * Removes the lower right fix point of the head sail
         */
        void RemoveSheetOfHeadSail();
        
        /**
         * Update the wind strength by the given value
         */
        void UpdateWindStrength(const float);
        
        /**
         * Initializes the actions that can be triggered in the
         * player component
         */
        virtual void VInitActions();
        
        /**
         * Starts playing a song
         */
        void PlayMusic();
        
        void VeerMainSail();
        
        void HaulMainSail();
        
        void VeerHeadSail();
        
        void HaulHeadSail();
        
        
    public:
        
        PlayerComponent() = default;
        
        virtual ~PlayerComponent()
        {
        }
        
        virtual void VInit();
        
        virtual string VGetIdentifier() const
        {
            return "playerComponent";
        }
        
        /*
         * Initializes the player component with the given XML data
         */
        virtual void VFetchData(StrongXMLElementPtr);
    };
    
}