#include "Interactive.h"
#include "../../application_layer/core/GameCore.h"
#include "../../game_logic/event_system/EvtData_KeyPressed.h"
#include "KeyboardSensor.h"

using namespace Game_View;

void Interactive::VInit()
{
    VInitActions();
    
    EventListenerDelegate checkForActiveSensors =
        fastdelegate::MakeDelegate(
                               this,
                               &Interactive::CheckForActiveSensors
                               );
    
    
    g_pGameApp->GetPlayerInputEvtMgr()->VAddListener(
                                                     checkForActiveSensors,
                                                     EvtData_KeyPressed::s_eventType);
}

void Interactive::PerformAction(const string& action)
{
    auto findIt = m_actions.find(action);
    
    if(findIt!=m_actions.end())
    {
        m_actions.at(action)();
    }
}

const string Interactive::GetActionForKey(const string& keyString)
{
    auto findIt = m_actionMapping.find(keyString);
    
    if(findIt!=m_actionMapping.end())
    {
        return m_actionMapping.at(keyString);
    }
    
    return "";
}

void Interactive::InitKeyLayout(
                        StrongXMLElementPtr pXmlElement)
{
    StrongXMLElementPtr pKeyElement =
                pXmlElement->FindFirstChildNamed("key");
    
    int index = 0;
    
    while (pKeyElement != nullptr)
    {
        string keyString =
        pKeyElement->GetValueOfAttribute("key");
        string action =
        pKeyElement->GetValueOfAttribute("action");
        
        InitSensorForAction(action, keyString);
        
        pKeyElement =
        pXmlElement->FindChildNamed("key", ++index);
    }
}

void Interactive::InitSensorForAction(
                    const string& action, const string& keyString)
{
    int glfwKey =
    KeyMapping::GetGLFWKeyForString(keyString);
    
    StrongSensorPtr pKeySensor(new KeyboardSensor());
    
    pKeySensor->AddListener(glfwKey);
    
    m_sensors.push_back(pKeySensor);
    m_actionMapping.insert(
                           pair<string, string>(keyString, action));
}

void Interactive::CheckForActiveSensors(IEventDataPtr pEventData)
{
    shared_ptr<EvtData_KeyPressed> pCastEventData =
    std::static_pointer_cast<EvtData_KeyPressed>(pEventData);
    
    
    if(pCastEventData->GetAction()==GLFW_RELEASE)
    {
        return ;
    }
    
    string keyString =
    KeyMapping::GetKeyStringForGLFW(pCastEventData->GetKey());
    
    if(keyString.compare("")==0)
    {
        return;
    }
    
    for(StrongSensorPtr pSensor : m_sensors)
    {
        if(pSensor->IsActive())
        {
            string action =
            GetActionForKey(keyString);
            
            if(action.compare("")!=0)
            {
                PerformAction(action);
            }
        }
    }
}