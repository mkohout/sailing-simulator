#pragma once

#include "../scene_graph/SceneGraph.h"
#include "../scene_graph/SceneGroup.h"
#include "../scene_graph/CameraNode.h"
#include "../audio/Sound.h"
#include "../../game_logic/data_structures/Settings.h"
#include "../../application_layer/resources/XMLResource.h"
#include "../../game_logic/actor_system/ActorFactory.h"
#include "../../game_logic/actor_system/Actor.h"
#include "../../application_layer/core/stdafx.h"
#include "../../game_logic/game_app/GameApp.h"
#include "../../game_logic/physics/Force.h"
#include "../../game_logic/event_system/EventManager.h"

#include <map>
#include <memory>
#include <vector>

#include <string>

using std::map;
using std::string;
using std::shared_ptr;
using std::vector;

using namespace Game_Logic;

namespace Game_View
{
    typedef map<string, StrongActorPtr> ActorCache;
    typedef map<ActorId, StrongActorPtr> ActorList;
    typedef vector<StrongSoundPtr> Sounds;
    
    /**
     * Class is representing a scene and contains
     * informations about actors and their relationships
     */
    class Scene
    {
    private:
        
        /**
         * The forces that are acting in the scene
         */
        Forces m_forces;
        
        /**
         * The background sounds that are played in the scene.
         */
        Sounds m_sounds;
        
        /*
         * The scene graph that holds all scene nodes in the
         * hierachical order that were used in the XML
         */
        SceneGraph* m_pSceneGraph;
        
        /*
         * All actors in the current scene, searchable by string
         */
        ActorCache m_actors;
        
        /*
         * The actors in the current scene, searchable by the actor id
         */
        ActorList m_actorList;
        
        /**
         * Pointer to the camera node in the scene graph
         */
        shared_ptr<CameraNode> m_pCameraNode;
    
        /*
         * Adds the forces given in the XML element to the current scene
         */
        void ProcessForces(StrongXMLElementPtr);
        
        /*
         * Adds the given force to the current scene
         */
        void AddForceToScene(StrongXMLElementPtr);
        
        /**
         * Caches the given actor to preserve it, in case there will be more of them
         */
        void AddActor(const string&, StrongActorPtr);
        
        /**
         * Adds the given actor to the list of the current scene so they can be referenced quickly
         */
        void AddActorToScene(StrongActorPtr);
        
        /*
         * Adds an actor to the given scene group
         */
        void AddActorToSceneGroup(StrongXMLElementPtr, StrongSceneNodePtr);
        
        /*
         * Adds a camera to the current scene
         */
        void AddCameraToScene(StrongXMLElementPtr, StrongSceneNodePtr);
        
        /*
         * Adds the given light to the current scene
         */
        void AddLightToScene(StrongXMLElementPtr, StrongSceneNodePtr);
        
        /*
         * Adds a SkyNode - e.g. a skybox - to the current scene
         */
        void AddSkyNodeToScene(StrongXMLElementPtr, StrongSceneNodePtr);
        
        /*
         * Based on the given XML element, this method will create
         * an actor in the current scene
         */
        StrongActorPtr CreateActor(const string&, StrongXMLElementPtr);
        
        /**
         * Extracts the informations of the given EventData and transforms
         * the actor that was specified by the event data using the given
         * transformation matrix
         */
        void ApplyTransformation(IEventDataPtr);
        
        /**
         * Initializes the event listeners that will change the
         * rotation, position and scale of the current actor
         * if the got an event to do so
         */
        void InitTransformationListeners();
        
    public:
        /*
         * Returns a pointer to the actor who has the flag isMainActor=true,
         * null otherwise
         */
        StrongActorPtr FindMainActor();

    private:
        /**
         * Initializes the forces that were added to the scene.
         * Dynamic forces will be initialized in this method.
         */
        void InitForces();
        
        /**
         * Processes the given xml element and adds the sounds
         * to the according data structure of the scene
         */
        void ProcessSounds(StrongXMLElementPtr);
        
        /**
         * Initializes the added sounds and start playing
         */
        void InitSounds();
        
    public:
        Scene() : m_forces(), m_pSceneGraph() { }
        ~Scene();
        
        /**
         * Returns the scene graph of the current scene
         */
        SceneGraph* GetSceneGraph() const
        {
            return m_pSceneGraph;
        }
        
        shared_ptr<CameraNode> GetCamera() const;
    
        /*
         * Returns true if the current scene has an actor with the given identifier
         */
        bool HasActorWithIdentifier(const string&);
        
        /**
         * Returns a pointer to the prototype of an actor identifier by the given string
         */
        StrongActorPtr GetActorWithIdentifier(const string&) const;
        
        /*
         * Returns an actor with the given actor id
         */
        StrongActorPtr GetActorWithActorId(ActorId) const;
        
        /*
         * Returns true if the actor with the given ActorId is in the current scene
         */
        bool HasActorWithActorId(ActorId);
        
        /**
         * Initializes the camera in the current scene, by centering
         * the camera to the main actor
         */
        void InitCamera();
        
        /*
         * Initializes the scene from by the given xml resource
         */
        void InitFromXMLResource(shared_ptr<XMLResource>);
        
        /*
         * Process a single xml element in the scenes xml resource
         */
        void ProcessXMLElement(StrongXMLElementPtr);
        
        /*
         * Process a single xml element and add its product to the given scene group parent
         */
        void ProcessXMLElement(StrongXMLElementPtr, StrongSceneNodePtr);
        
        /*
         * Returns a vector with all forces that are acting in the scene
         */
        Forces GetForces() const;
        
        /**
         * Returns a pointer to the force that is acting in the scene with the given
         * name
         */
        StrongForcePtr FindSpecificForce(const string&) const;
        
        /**
         * Update method that changes the forces according to their
         * specfied dynamics
         */
        void UpdateForces(const unsigned int&);
    };
}
