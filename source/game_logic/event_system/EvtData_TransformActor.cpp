#include "EvtData_TransformActor.h"
#include <glfw/glfw3.h>

using namespace Game_Logic;

const EventType EvtData_TransformActor::s_eventType = 0x5471CAF5;

EvtData_TransformActor::EvtData_TransformActor(
                            ActorId actorId, mat4 transform)
    : m_actorId(actorId), m_matrix(transform)
{
}

void EvtData_TransformActor::SetApplyTransformationToParent(
                                bool transformParent)
{
    m_applyTransformationToParent = transformParent;
}