#pragma once

#include <vector>
#include <memory>
#include <string>

#include "../../application_layer/core/stdafx.h"

#include "../../application_layer/resources/XMLElement.h"

using std::string;
using std::weak_ptr;
using std::shared_ptr;

using namespace Application_Layer;

namespace Game_Logic
{
	class Actor;
    class ActorFactory;

	typedef weak_ptr<Actor> WeakActorPtr;
	typedef shared_ptr<Actor> StrongActorPtr;

	/**
	 * Baseclass for all actorcomponents
	 */
	class ActorComponent
	{
    private:
        friend ActorFactory;
        
	protected:
		WeakActorPtr m_pActor;

		/*
		 * Sets the actor of the current component
		 */
		void SetActor(StrongActorPtr pActor)
		{
			m_pActor = pActor;
		}

		/*
		 * Returns a pointer to the current actor
		 */
		StrongActorPtr GetActor()
		{
			return StrongActorPtr(m_pActor);
		}

	public:
		virtual ~ActorComponent() { }

		/*
		 * Initializes the actor component
		 */
		virtual void VInit() { }

		/*
		 * Returns the identifier of the current component
		 */
		virtual string VGetIdentifier() const
		{
			return "Undefined";
		}

		/*
		 * Initializes the component by the given xml element
		 */
		virtual void VFetchData(StrongXMLElementPtr) { }
	};

}