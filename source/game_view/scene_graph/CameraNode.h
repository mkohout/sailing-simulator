#pragma once

#include "ISceneNode.h"

#include "../../application_layer/core/stdafx.h"
#include "../../game_logic/event_system/EventManager.h"
#include "../ui/MouseSensor.h"
#include "../ui/Interactive.h"

#include <mutex>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

#define MAX_ZOOM_DISTANCE 100.0f
#define DEFAULT_CAMERA_SPEED 1.0f

namespace Game_View
{
    /**
     * Node for representing the camera
     */
    class CameraNode : public ISceneNode
    {
    private:
        /**
         * Camera cannot store any child nodes, list will always be empty
         */
        SceneNodes m_childNodes;
        
        MouseSensor* m_pMoveSensor;
        
        MouseSensor* m_pRotationSensor;
        
        MouseSensor* m_pZoomInSensor;
        
        /**
         * The worlds upwards direction
         */
        vec3 m_up;
        
        /**
         * The point the camera is looking at
         */
        vec3 m_lookAt;
        
        /**
         * Position of the camera node
         */
        vec3 m_position;
        
        /**
         * TODO
         */
        vec3 m_originalPosition; //TODO
        
        /**
         * The speed the camera moves if no parameter
         * is given to the move function
         */
        float m_cameraMovingSpeed;
        
        /**
         * Values of the last measured mouse position in
         * screen space coordinates. Initial value is -1 to
         * avoid big steps in the movement
         */
        double m_lastMouseX;
        double m_lastMouseY;
        
        /**
         * The maximum distance between the look at position
         * and the camera position
         */
        float m_maxZoomDistance;
        
        /**
         * Pointer to the parent node of the camera
         */
        StrongSceneNodePtr m_pParent;
        
        /**
         * Far clipping plan of the camera, in float => distance from eye
         */
        float m_farClippingPlane;
        
        /**
         * Near clipping plan of the camera, in float => distance from eye
         */
        float m_nearClippingPlane;
        
        /**
         * The angle for the field of view of the current camera
         */
        float m_fieldOfView;
        
        /**
         * Initializes the events the camera is listening
         * to, such as the main actor movement for the 
         * third person camera
         */
        void InitListeners();
        
    public:
        /**
         * TODO
         */
        void FollowActor(StrongActorPtr);
        
    private:
        /**
         * Function that is triggered if the mouse has been moved
         * or a mouse button has been clicked
         */
        void MouseAction(IEventDataPtr);
        
        /**
         * Function that will move the camera according to the
         * measured delta values of the mouse coordinates
         */
        void MoveCameraTriggered(IEventDataPtr);
        
        /**
         * Function that will rotate the camera according to the
         * measured delta values of the mouse coordinates
         */
        void RotateCameraTriggered(IEventDataPtr);
        
        /**
         * Function that zooms that shortens the distance between
         * the current position of the camera and the point the camera is
         * looking at
         */
        void ZoomInTriggered(IEventDataPtr);
        
        /**
         * Initializes the mouse events the camera uses for the
         * camera control
         */
        void InitMouseEvents();
        
    public:
        CameraNode();
        
        virtual ~CameraNode()
        {
            SAFE_DELETE(m_pMoveSensor);
            SAFE_DELETE(m_pRotationSensor);
            SAFE_DELETE(m_pZoomInSensor);
        }
        
        /**
         * Initializes the camera node by setting the action listener etc.
         */
        virtual void VInit();
        
        /**
         * Initializes the camera node by a given xml element
         */
        virtual void VInitByXMLElement(StrongXMLElementPtr);
        
        /**
         * CameraNodes cannot have child nodes
         */
        virtual void VAddChildNode(StrongSceneNodePtr pSceneNode);
        
        /**
         * CameraNodes cannot have child nodes
         */
        virtual void VRemoveChildNode(StrongSceneNodePtr pSceneNode);
        
        /**
         * Sets the transformation matrix for the camera
         */
        virtual void VSetTransformationMatrix(const mat4&);
        
        /**
         * Performs a transform on the current camera
         * means, that the current transformation matrix
         * will be multiplied with the given
         */
        virtual void VTransform(const mat4&);
        
        /**
         * Returns the current transformation matrix
         */
        virtual const mat4 VGetTransformationMatrix() const;
        
        /**
         * Camaera Node is not able to store child nodes
         */
        virtual SceneNodes& VGetAllChildren();
        
        /**
         * CameraNode cannot have child nodes
         */
        virtual bool VHasChildren() const
        {
            return false;
        }
        
        /**
         * Camaera Node is not able to store an actor
         */
        virtual StrongActorPtr VGetActor() const;
        
        /**
         * Camaera Node is not able to store an actor
         */
        virtual bool VHasActor() const;
        
        void SetUp(float, float, float);
        
        void SetPosition(float, float, float);
        
        std::mutex g_pages_mutex;
        
        void SetPosition(vec3);
        
        void SetFarClippingPlane(float);
        
        void SetNearClippingPlane(float);
        
        void SetFieldOfView(float);
        
        void SetLookAt(float, float, float);
        
        void SetLookAt(vec3);
        
        /**
         * Returns the view matrix, the camera is representing
         */
        const mat4 GetViewMatrix() const;
        
        /**
         * Returns the projection matrix calculated on the parameters given by the camera node xml data
         */
        const mat4 GetProjectionMatrix(float) const;

		float GetFieldOfView() const
		{
			return m_fieldOfView;
		}

		float GetNearClippingPlane() const
		{
			return m_nearClippingPlane;
		}

		float GetFarClippingPlane() const
		{
			return m_farClippingPlane;
		}
        
		virtual vec3 VGetPosition() const;

		vec3 GetDirection() const
		{
			return normalize(m_lookAt - VGetPosition());
		}
        
        /**
         * Since the camera node is not able to store an actor
         * this function is a blank
         */
        virtual void VSetActor(StrongActorPtr)
        {
        }
        
        /**
         * Sets the given scene ndoe as the parent of the
         * current
         */
        virtual void VSetParent(StrongSceneNodePtr);
        
        /**
         * Returns the parent of the current camera node
         */
        virtual StrongSceneNodePtr VGetParent() const
        {
            return m_pParent;
        }
    };
}