#pragma once

#include "IPhysicsComponent.h"
#include "ActorComponent.h"

#include <string>
#include <glm/glm.hpp>

using namespace glm;

namespace Game_Logic
{
    /**
     * Component that manages the dynamic forces and
     * specialities happening around a sail.
     */
    class SailComponent : public ActorComponent
    {
    private:
        /**
         * Name of the actor where the force that acts
         * on the sail will be applied to
         */
        std::string m_applyForceOnActor;
        
        /**
         * Offset where the force will be applied to
         */
        glm::vec3 m_offset;
        
        /**
         * The point of the sail where the rope is fixed to
         */
        glm::vec3 m_sheetPoint;
        
        /**
         * The other end of the fictive rope that is connected
         * to the sheet and tighten or loose the sail
         */
        glm::vec3 m_towPoint;
        
        enum COURSE
        {
            HEAD_TO_WIND, // Im wind
            CLOSE_HAULED, // hart am wind
            CROSSING_WIND, // halbwind
            BROAD_REACH, // Raumschots
            WIND_RIGHT_AFT // vor dem wind / wind von achtern
        };
        
        /**
         * According to the current wind course, the speed
         * will be multiplied by a specific factor, caused
         * by friction etc.
         */
        float GetSpeedMultiplierAccordingToWindCourse(COURSE);
        
        /**
         * Returns the wind course according to the
         * given angle between the pointing-to-vector
         * and the wind direction
         */
        COURSE GetWindCourseFromAngle(float);
        
        /**
         * Returns the pointing-to vector,
         * if its the zero-vector, then the pointing-to
         * vector will be calculated first
         */
        const vec3 GetPointingToVector();
        
        
        /**
         * Returns the direction of the wind, that
         * is currently acting in the scene
         */
        const vec3 GetWindDirection() const;
        
        /**
         * Returns the strength of the wind, that
         * is currently acting in the scene
         */
        const float GetWindStrength() const;
        
        /**
         * TODO
         */
        const vec3 CalculateForceVector(const glm::vec3);
        
        /**
         * Returns the angle of the current wind direction
         * and the direction the boat is pointing
         * to to determine the wind course.
         */
        const float GetAngleBetweenWindAndPointingTo(
                                        const glm::vec3, const glm::vec3);
        
    public:
        SailComponent() = default;
        
        virtual ~SailComponent()
        {
            
        }
        
        virtual void VInit() { }
        
        /**
         * Initializes the current component by with the properties
         * given in the xml element
         */
        virtual void VFetchData(StrongXMLElementPtr);
        
        virtual string VGetIdentifier() const
        {
            return "sailComponent";
        }
        
        /**
         * Apply the calculated force to the target actor
         */
        virtual void ApplyForce(const vec3);
        
        /**
         * TODO
         */
        void VeerSail();
        
        /**
         * TODO
         */
        void HaulSail();
    };
    
}