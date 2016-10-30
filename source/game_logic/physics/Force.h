#pragma once

#include <vector>
#include <memory>
#include <string>
#include "../event_system/IEventData.h"
#include "../../application_layer/resources/XMLElement.h"
#include <glm/vec3.hpp>


using namespace glm;
using namespace Application_Layer;

namespace Game_Logic
{
    /**
     * Class that represents a force that is acting
     * in the scene. A force has a strength and a direction
     * and therefore applies on all actors in the scene in the same manner.
     */
    class Force
    {
    private:
        
        /**
         * The force can be dependent on the actor, e.g. the airstream
         * if this string is not empty, the force will listen to the actor
         * moved event and will update its direction and strength
         */
        std::string m_dependentOnActor;
        
        /**
         * enum to specify the different types of forces
         */
        enum EFFECT
        {
            AREA,
            PARTICLE
        };

        /**
         * enum to specify if the force is a static one,
         * or a dynamic that will change its direction or strength
         * over time or at specific events
         */
        enum TYPE
        {
            STATIC,
            DYNAMIC
        };
        
        /**
         * The current direction of the current force.
         * may change if the force is dynamic.
         */
        vec3 m_direction;
        
        /**
         * The strength of the current force.
         * Is the base for the calculations for forces
         * that may change over time
         */
        float m_strength;
        
        /**
         * The temporary strength that will be calculated
         * every frame/time it is called
         */
        float m_tempStrength;
        
        /**
         * The maximum value the strength can have - 0.0f if no
         * value was specified
         */
        float m_maxStrength;
        
        /**
         * The identifier of the force
         */
        string m_identifier;
        
        /**
         * Indicates whether the current force is mass
         * independent, e.g. gravity, etc.
         */
        bool m_massIndependent;
        
        /**
         * The angle the force is currently rotated.
         * 0.0f if the force is static
         */
        float m_currentRotation;
        
        /**
         * TODO
         */
        vec3 m_rotation;
        
        /**
         * The current value of the strength, the base (m_strength)
         * and the dynamics added (if any were specified)
         */
        float m_currentStrengthValue;
        
        /**
         * The noise that will be added/subtracted
         * by the sine wave (amplitude of the sine wave)
         */
        float m_strengthNoise;
        
        /**
         * The type of the current force:
         * static or dynamic
         */
        TYPE m_type;
        
        /**
         * The effect of the current force, area or particle
         */
        EFFECT m_effect;
        
        void SetDirection(
                    const std::string&,
                    const std::string&,
                    const std::string&);
        
        /**
         * Sets the direction of the current force to the given
         */
        void SetDirection(const glm::vec3);
        
        /**
         * Initializes the input parameters for the dynamic
         * force from the given xml element
         */
        void InitDynamics(StrongXMLElementPtr);
        
        /**
         * If the strength has changed, an event will be
         * triggered in this method to manipulate the volume
         * according to the new force strength
         */
        void TriggerChangeAudioEvent();
        
        /**
         * Initializes the event listeners that will be triggered
         * whenever an actor is moved
         */
        void InitMovementListenerForActorIdentifier();
        
        /**
         * Callback function that will be executed whenever
         * the event ActorHasMoved was triggered. If the actor
         * the force is depending on was moved, then the TriggerChangeAudioEvent
         * will be executed
         */
        void ActorMoved(IEventDataPtr);
        
    public:
        
        Force() = default;
        
        ~Force() { }
        
        /**
         * Initializes the current force
         */
        void Init();

        /**
         * Initializes the force by the given xml element
         */
        void InitFromXMLElement(StrongXMLElementPtr);
        
        inline const float GetStrength() const
        {
            return (m_type==TYPE::STATIC ? m_strength : m_tempStrength);
        }
        
        bool IsMassIndependent() const
        {
            return m_massIndependent;
        }
        
        void SetStrength(const float);
        
        void SetDirection(const float& x, const float& y, const float& z)
        {
            m_direction = vec3(x, y, z);
        }
        
        inline const vec3 GetDirection() const
        {
            return m_direction;
        }
        
        inline const string GetIdentifier() const
        {
            return m_identifier;
        }
        
        const float GetStrengthWithoutNoise() const
        {
            return m_strength;
        }
        
        const float GetMaxStrength() const
        {
            return m_maxStrength;
        }
        
        /**
         * Returns true whether the given
         * force is an area force, where
         * the normals needs to be calculated
         * before the force can be applied
         */
        bool IsAreaForce() const
        {
            return m_effect==EFFECT::AREA;
        }
        
        /**
         * Update method that will be called every time the render method 
         * in the game app will be executed.
         */
        void Update(const unsigned int&);
    };
    
    typedef std::shared_ptr<Force> StrongForcePtr;
    typedef std::vector<StrongForcePtr> Forces;
}