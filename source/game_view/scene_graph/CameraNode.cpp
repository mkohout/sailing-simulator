#include "CameraNode.h"
#include "../../application_layer/core/GameCore.h"
#include "../../game_logic/event_system/EvtData_MouseMoved.h"
#include "../../game_logic/event_system/EvtData_MouseClicked.h"
#include "../../game_logic/event_system/EvtData_Scrolled.h"
#include "../../game_logic/event_system/EvtData_ActorHasMoved.h"
#include "../ui/ANDController.h"
#include <glfw/glfw3.h>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream> //TODO

using namespace Game_View;

CameraNode::CameraNode() : m_lastMouseX(-1.0), m_lastMouseY(-1.0)
{
    m_position = vec3(0, 0, 0);
}

void CameraNode::VInitByXMLElement(StrongXMLElementPtr pXmlElement)
{
	if (pXmlElement->HasChildrenNamed("position"))
	{
		StrongXMLElementPtr pPositionElement =
			pXmlElement->FindFirstChildNamed("position");

		SetPosition(
			std::stof(pPositionElement->GetValueOfAttribute("x")),
			std::stof(pPositionElement->GetValueOfAttribute("y")),
			std::stof(pPositionElement->GetValueOfAttribute("z"))
			);
        
        m_originalPosition = m_position;
	}
    
    if(pXmlElement->HasChildrenNamed("movingSpeed"))
    {
        StrongXMLElementPtr pMovingSpeedElement =
                pXmlElement->FindFirstChildNamed("movingSpeed");
        
        m_cameraMovingSpeed =
                    std::stof(pMovingSpeedElement->GetValueOfAttribute("value"));
    } else
    {
        m_cameraMovingSpeed = DEFAULT_CAMERA_SPEED;
    }

	if (pXmlElement->HasChildrenNamed("nearClippingPlane"))
	{
		StrongXMLElementPtr pNearClippingPlaneElement =
			pXmlElement->FindFirstChildNamed("nearClippingPlane");

		SetNearClippingPlane(
			std::stof(
				pNearClippingPlaneElement->GetValueOfAttribute("value")));
	}
    
    if(pXmlElement->HasChildrenNamed("maxZoomDistance"))
    {
        StrongXMLElementPtr pMaxZoomDistanceElement =
                    pXmlElement->FindFirstChildNamed("maxZoomDistance");
        
        m_maxZoomDistance =
                std::stof(pMaxZoomDistanceElement->GetValueOfAttribute("value"));
    }
    else
    {
        m_maxZoomDistance = MAX_ZOOM_DISTANCE;
    }
    
    if (pXmlElement->HasChildrenNamed("up"))
    {
        StrongXMLElementPtr upElement =
        pXmlElement->FindFirstChildNamed("up");
        
        SetUp(
              std::stof(upElement->GetValueOfAttribute("x")),
              std::stof(upElement->GetValueOfAttribute("y")),
              std::stof(upElement->GetValueOfAttribute("z")));
    }

	if (pXmlElement->HasChildrenNamed("farClippingPlane"))
	{
		StrongXMLElementPtr pFarClippingPlaneElement =
			pXmlElement->FindFirstChildNamed("farClippingPlane");

		SetFarClippingPlane(
			std::stof(pFarClippingPlaneElement->GetValueOfAttribute("value")));
	}

	if (pXmlElement->HasChildrenNamed("fieldOfView"))
	{
		StrongXMLElementPtr pFieldOfViewElement =
			pXmlElement->FindFirstChildNamed("fieldOfView");

		SetFieldOfView(
			std::stof(pFieldOfViewElement->GetValueOfAttribute("value")));
	}
    
    VInit();
}

void CameraNode::VInit()
{
    InitListeners();
}

void CameraNode::InitListeners()
{
    InitMouseEvents();
}

void CameraNode::FollowActor(StrongActorPtr pActor)
{
    if(!pActor->IsMainActor())
    {
        return ;
    }
    
    StrongSceneNodePtr pSceneNode =
                    g_pGameApp->GetCurrentScene()
                        ->GetSceneGraph()->FindNodeWithActor(
                                            pActor->GetActorId());
    
    const mat4 transform = pSceneNode->VGetTransformationMatrix();
    vec3 translation = vec3(transform * vec4(1));
	vec3 diff = translation - m_lookAt;
	diff.y = 0.0f;
	m_lookAt += diff;
	m_position += diff;
}

void CameraNode::InitMouseEvents()
{
    //TODO, refactoring using interactive class
    m_pRotationSensor = new MouseSensor();
    m_pZoomInSensor = new MouseSensor();
    
    m_pRotationSensor->SetController(new ANDController());
    
    m_pRotationSensor->AddListener(GLFW_MOUSE_BUTTON_MIDDLE);
    m_pRotationSensor->AddListener(MouseSensor::MOUSE_MOVED);
    
    m_pZoomInSensor->AddListener(MouseSensor::SCROLLED);
    
    EventListenerDelegate mouseTriggerFunction =
    fastdelegate::MakeDelegate(
                               this,
                               &CameraNode::MouseAction
                               );
    EventListenerDelegate zoomTriggerFunction =
    fastdelegate::MakeDelegate(
                               this,
                               &CameraNode::ZoomInTriggered
                               );
    
    g_pGameApp->GetPlayerInputEvtMgr()->VAddListener(
                                                     mouseTriggerFunction,
                                                     EvtData_MouseMoved::s_eventType);
    g_pGameApp->GetPlayerInputEvtMgr()->VAddListener(
                                                     mouseTriggerFunction,
                                                     EvtData_MouseClicked::s_eventType);
    g_pGameApp->GetPlayerInputEvtMgr()->VAddListener(
                                                     zoomTriggerFunction,
                                                     EvtData_Scrolled::s_eventType);
}

void CameraNode::MouseAction(IEventDataPtr pEventData)
{
    if (dynamic_cast<EvtData_MouseMoved*>(pEventData.get()) == 0)
    {
        return;
    }

	if (m_pRotationSensor->IsActive())
	{
		RotateCameraTriggered(pEventData);
	}
    else
    {
        m_lastMouseX = -1.0;
        m_lastMouseY = -1.0;
    }
}

void CameraNode::ZoomInTriggered(IEventDataPtr pEventData)
{
    if(!m_pZoomInSensor->IsActive())
    {
        return ;
    }
    
    shared_ptr<EvtData_Scrolled> pCastEventData =
            std::static_pointer_cast<EvtData_Scrolled>(pEventData);
    
    // Straight line from the camera position to the look at
    // New Camera position is depending on the scroll direction
    
    double stepSize = 0.1;
    double lambda =
        (pCastEventData->GetScrollOffsetY()>0 ? 1 : -1) * stepSize;
    
    vec3 position = VGetPosition();
    vec3 diff = m_lookAt-position;
    diff *= lambda;
    vec3 newCameraPosition = position + diff;
    
    if(glm::length(newCameraPosition-m_lookAt)<m_maxZoomDistance)
    {
		m_position = newCameraPosition;
    }
    
    m_pZoomInSensor->SetActiveValue(MouseSensor::SCROLLED, false);
}

void CameraNode::RotateCameraTriggered(IEventDataPtr pEventData)
{
	shared_ptr<EvtData_MouseMoved> pCastEventData =
		std::static_pointer_cast<EvtData_MouseMoved>(pEventData);

	double currentX = pCastEventData->GetX();
	double currentY = pCastEventData->GetY();

	if (m_lastMouseX != -1.0 && m_lastMouseY != -1.0)
	{
		double differenceX = m_lastMouseX - currentX;
		double differenceY = m_lastMouseY - currentY;

		float AngleAddX = glm::radians((float)differenceX / 3), AngleAddY = -glm::radians((float)differenceY / 3);

        vec3 position = VGetPosition();
		vec3 forward = normalize(m_lookAt - position);
		vec3 right = normalize(cross(m_up, forward));

		mat4 tr = glm::rotate(AngleAddY, right) * glm::rotate(AngleAddX, vec3(0, 1, 0));
		auto mat = glm::translate(m_lookAt) * tr * glm::translate(-m_lookAt);
		m_position = vec3(mat * vec4(m_position, 1));
	}

	m_lastMouseX = currentX;
	m_lastMouseY = currentY;
}

void CameraNode::VAddChildNode(StrongSceneNodePtr pSceneNode)
{
}

void CameraNode::VRemoveChildNode(StrongSceneNodePtr pSceneNode)
{
}

void CameraNode::VSetTransformationMatrix(const mat4& transform)
{
	VTransform(transform * glm::inverse(VGetTransformationMatrix()));
}

void CameraNode::VTransform(const mat4& transform)
{
	m_position = vec3(transform * vec4(m_position, 1));
	m_lookAt = vec3(transform * vec4(m_lookAt, 1));
	m_up = vec3(transform * vec4(m_up, 0));
}

const mat4 CameraNode::VGetTransformationMatrix() const
{
	return glm::inverse(GetViewMatrix());
}

SceneNodes& CameraNode::VGetAllChildren()
{
	return m_childNodes;
}

StrongActorPtr CameraNode::VGetActor() const
{
	return nullptr;
}

bool CameraNode::VHasActor() const
{
	return false;
}

void CameraNode::SetPosition(float x, float y, float z)
{
    SetPosition(vec3(x, y, z));
}

void CameraNode::SetPosition(vec3 newPosition)
{
    g_pages_mutex.lock();
    
	auto diff = newPosition - m_position;    
    m_position = newPosition;
	m_lookAt += diff;
    
    g_pages_mutex.unlock();
}

vec3 CameraNode::VGetPosition() const
{
    return m_position;
}

void CameraNode::SetFarClippingPlane(float value)
{
	m_farClippingPlane = value;
}

void CameraNode::SetNearClippingPlane(float value)
{
	m_nearClippingPlane = value;
}

void CameraNode::SetFieldOfView(float value)
{
	m_fieldOfView = value;
}

void CameraNode::SetUp(float x, float y, float z)
{
    m_up = vec3(x, y, z);
}

void CameraNode::SetLookAt(float x, float y, float z)
{
    SetLookAt(vec3(x, y, z));
}

void CameraNode::SetLookAt(vec3 lookAt)
{
    m_lookAt = lookAt;
}

const mat4 CameraNode::GetViewMatrix() const
{
	return lookAt(
		VGetPosition(),
		m_lookAt,
		m_up
		);
}

const mat4 CameraNode::GetProjectionMatrix(float aspect) const
{
	float fov = m_fieldOfView;
#ifdef GLM_FORCE_RADIANS
	fov = glm::radians(m_fieldOfView);
#endif
	return perspective(
		fov,
        aspect,
        m_nearClippingPlane,
        m_farClippingPlane
        );
}

void CameraNode::VSetParent(StrongSceneNodePtr pSceneNode)
{
    m_pParent = pSceneNode;
}