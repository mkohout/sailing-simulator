#include "PhysicsComponent.h"
#include <iostream>
#include "../actor_system/Actor.h"
#include "../physics/BulletPhysics.h"
#include "../event_system/EvtData_TransformActor.h"
#include "../event_system/EvtData_ActorHasMoved.h"
#include <glm/gtc/matrix_transform.hpp>

#include "../../application_layer/core/GameCore.h"
#include "../../application_layer/resources/ResourceManager.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

using namespace Game_Logic;
using namespace std;

void PhysicsComponent::VInit()
{
    //TODO
    m_pointingTo = btVector3(1.0f, 0.0f, 0.0f);
}

glm::vec3 getScale(const glm::mat4& m)
{
	glm::vec3 col1(m[0][0], m[0][1], m[0][2]);
	glm::vec3 col2(m[1][0], m[1][1], m[1][2]);
	glm::vec3 col3(m[2][0], m[2][1], m[2][2]);
	//Extract the scaling factors
	glm::vec3 scaling;
	scaling.x = glm::length(col1);
	scaling.y = glm::length(col2);
	scaling.z = glm::length(col3);
	return scaling;
}

void PhysicsComponent::VUpdate(const unsigned int deltaMilliseconds)
{
	if(rigidBody == nullptr)
	{
		StrongActorPtr pActor(m_pActor);
		btTransform tr;
		auto mat = g_pGameApp->GetCurrentScene()->GetSceneGraph()->FindNodeWithActor(pActor->GetActorId())->VGetTransformationMatrix();
		tr.setFromOpenGLMatrix(reinterpret_cast<float*>(&mat));
		motionState = new btDefaultMotionState(tr);

		btVector3 inertia(0, 0, 0);
		shape->calculateLocalInertia(mass, inertia);


		glm::vec3 scale = getScale(mat);
		shape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, shape, inertia);

		rigidBody = new btRigidBody(rigidBodyCI);
		rigidBody->setDamping(0.95, 0.9);
		rigidBody->activate();
		rigidBody->forceActivationState(rigidBody->getActivationState());
		BulletPhysics::GetInstance()->GetDynamicsWorld()->addRigidBody(rigidBody);

		m_bouyancyDirection = -(g_pGameApp->GetCurrentScene()->FindSpecificForce("gravity")->GetDirection());
		m_gravityStrength = g_pGameApp->GetCurrentScene()->FindSpecificForce("gravity")->GetStrength();
	}
	else
	{
		btTransform trans;
		rigidBody->getMotionState()->getWorldTransform(trans);
		auto& p = trans.getOrigin();
		auto r = trans.getRotation();

		auto waterActor = g_pGameApp->GetCurrentScene()->GetActorWithIdentifier("water");
		auto pComponent = dynamic_pointer_cast<OceanComponent>(waterActor->GetComponent("oceanComponent"));


		if(m_pGeometry != nullptr)
		{
			auto& vertices = m_pGeometry->GetAllVertexData();
			auto& indices = m_pGeometry->GetIndices();
			StrongActorPtr pActor(m_pActor);
			auto mat = g_pGameApp->GetCurrentScene()->GetSceneGraph()->FindNodeWithActor(pActor->GetActorId())->VGetTransformationMatrix();
			for (unsigned i = 0; i < indices.size(); i+=3)
			{
				auto& vertex1 = vertices[indices[i]].Position;
				auto& vertex2 = vertices[indices[i+1]].Position;
				auto& vertex3 = vertices[indices[i+2]].Position;

				auto vertex1_g = glm::vec3(mat*glm::vec4(vertex1, 1));
				auto vertex2_g = glm::vec3(mat*glm::vec4(vertex2, 1));
				auto vertex3_g = glm::vec3(mat*glm::vec4(vertex3, 1));
				float min_y = glm::min(vertex1_g.y, glm::min(vertex2_g.y, vertex3_g.y));

				auto mid_point = (vertex1 + vertex2 + vertex3)/3.0f;
				auto global_mid_point = (vertex1_g + vertex2_g + vertex3_g) / 3.0f;

				auto height = pComponent->GetHeightAtPoint(global_mid_point);
				auto normal_g = glm::cross(glm::normalize(vertex2_g - vertex1_g), glm::normalize(vertex3_g - vertex1_g));
				auto multiplier = glm::dot(normal_g, -m_bouyancyDirection);
				if (multiplier > 0) {
					if(height > min_y)
					{
						std::vector<float> toSort;
						toSort.push_back(vertex1_g.y);
						toSort.push_back(vertex2_g.y);
						toSort.push_back(vertex3_g.y);
						std::sort(toSort.begin(), toSort.end());

						float fraction = 1.0f;
						float h01 = toSort[0] - toSort[1];
						float h02 = toSort[0] - toSort[2];
						float h12 = toSort[1] - toSort[2];
						if (height < toSort[1]) {
							float h0w = toSort[0] - height;
							fraction = (h0w * h0w) / (h01 * h02);
						}
						else if (height < toSort[2]) {
							float hw2 = height - toSort[2];
							fraction = 1 - (hw2 * hw2) / (h12 * h02);
						}


						auto rel_pos = btVector3(global_mid_point.x, global_mid_point.y, global_mid_point.z) - rigidBody->getWorldTransform().getOrigin();
						auto area = glm::length(glm::cross(vertex3_g - vertex1_g, vertex2_g - vertex1_g)) / 2;
						area *= fraction;
						auto volume = area * (height - global_mid_point.y)*multiplier;


						glm::vec3 force = m_bouyancyDirection*(volume*m_gravityStrength*1000)*0.6f;
						rigidBody->applyForce(btVector3(force.x, force.y, force.z), rel_pos);

						btVector3 v_btPoint = rigidBody->getVelocityInLocalPoint(btVector3(mid_point.x, mid_point.y, mid_point.z));
						glm::vec3 v_point = glm::vec3(v_btPoint.getX(), v_btPoint.getY(), v_btPoint.getZ());
						glm::vec3 v_water = pComponent->GetVelocityAtPoint(global_mid_point);

						auto U = v_point - v_water;
						auto A = glm::abs(glm::dot(normal_g, U) / glm::length(U) * area);

						auto drag = -0.5f * 1000 * A * glm::length(U) * U * 0.01f;
						drag *= fraction;
						//rigidBody->applyForce(btVector3(drag.x, drag.y, drag.z), rel_pos);

						auto lift = -0.5f * 1000 * A * glm::length(U) * (glm::cross(U , (glm::cross(normal_g, U) / glm::length(glm::cross(normal_g, U))))) * 0.01f;
						lift *= fraction;
						//rigidBody->applyForce(btVector3(lift.x, lift.y, lift.z), rel_pos);

						float val = glm::dot(normal_g, U);
						if (fraction < 1.0f && val > 0.0f /*&& glm::length(U) > 2.5f*/)
						{
							vec3 g_up(0, 1, 0);
							vec3 projectedUp = glm::normalize(g_up - glm::dot(g_up, normal_g)*normal_g);
							glm::vec3 direction = glm::normalize(glm::cross(glm::vec3(1, 0, 0), normal_g));
							//float strength = glm::length(U)/**(-glm::dot(normal_g, glm::normalize(U)))*/;
							StrongActorPtr pActor(m_pActor);
							glm::vec3 pos;
							if (vertex1_g.y == min_y)
								pos = vertex1_g;
							else if (vertex2_g.y == min_y)
								pos = vertex2_g;
							else if (vertex3_g.y == min_y)
								pos = vertex3_g;
							auto pParticleSystem = std::dynamic_pointer_cast<ParticleSystemComponent> (pActor->GetComponent("particleSystemComponent"));
							pParticleSystem->CreateSpray(5, projectedUp*val*1.2f, pos, 30, false);
						}
					}
					else {
						auto rel_pos = btVector3(global_mid_point.x, global_mid_point.y, global_mid_point.z) - rigidBody->getWorldTransform().getOrigin();
						auto force = -m_bouyancyDirection * 1500.0f;
						rigidBody->applyForce(btVector3(force.x, force.y, force.z), rel_pos);
					}
				}
			}
			auto force = -m_bouyancyDirection * 100000.0f;
			rigidBody->applyCentralForce(btVector3(force.x, force.y, force.z));
			
		}else
		{
			auto height = pComponent->GetHeightAtPoint(glm::vec3(p.getX(), p.getY(), p.getZ()));
			if (p.getY() <= height)
			{
				rigidBody->applyCentralForce(btVector3(0, (height - p.getY())*5, 0));
			}
		}
		rigidBody->activate(true);
	}
}

void PhysicsComponent::VFetchData(StrongXMLElementPtr pXmlElement)
{
	
	if(pXmlElement->HasChildrenNamed("shape"))
	{
		auto pXmlShape = pXmlElement->FindFirstChildNamed("shape");
		auto typeString = pXmlShape->GetValueOfAttribute("type");

		if (pXmlElement->HasChildrenNamed("mass"))
		{
			mass = std::stof(pXmlElement->FindFirstChildNamed("mass")->GetValueOfAttribute("value"));
		}
		else
		{
			mass = 1.0f;
		}

		if(typeString.compare("sphere") == 0)
		{
			auto radius = std::stof(pXmlShape->GetValueOfAttribute("radius"));
			shape = new btSphereShape(radius);
		}
		else if(typeString.compare("mesh")==0)
		{
			string objFile = pXmlShape->GetValueOfAttribute("source");
			string objFilename =
				g_pGameApp->GetFileManager()->GetPathToObjectFile(objFile);

			StrongResourcePtr pResource =
				g_pGameApp->GetResourceManager()->GetResource(objFilename, IResource::OBJ);
			shared_ptr<ObjResource> pObjResource =
				std::static_pointer_cast<ObjResource>(pResource);

			m_pGeometry = StrongGeometryPtr(pObjResource->GetCreatedGeometry()); 


			auto AABB = m_pGeometry->GetLocalAABB();
			auto vec = glm::max(AABB.max - AABB.Center(), AABB.min - AABB.Center());
			auto* box1 = new btBoxShape(btVector3(vec.x, vec.y, vec.z));

			shape = box1;
		}
		
	}else
	{
		shape = new btSphereShape(1);
	}
}

const glm::vec3 PhysicsComponent::GetPointingTo() const
{
    btVector3 pointingTo =
                rigid_body()->getWorldTransform().getBasis() * m_pointingTo;
    
    return glm::vec3(pointingTo.x(), pointingTo.y(), pointingTo.z());
}

void PhysicsComponent::ApplyForceToActor(const glm::vec3& force, const glm::vec3& offset)
{
    btVector3 transformedForce =
            rigid_body()->getWorldTransform().getBasis()
                * btVector3(force.x, force.y, force.z);
    btVector3 offsetPosition =
            rigid_body()->getWorldTransform().getBasis() *
            btVector3(offset.x, offset.y, offset.z);
    btVector3 relPosition =
                offsetPosition-rigidBody->getWorldTransform().getOrigin();
    
    rigid_body()->applyForce(transformedForce, relPosition);
}

void PhysicsComponent::ApplyForceToActor(const glm::vec3& force)
{
    rigid_body()->applyCentralForce(
        rigid_body()->getWorldTransform().getBasis()
            * btVector3(force.x, force.y, force.z));
}

const glm::vec3 PhysicsComponent::GetVelocity() const
{
    btVector3 velocity = rigid_body()->getLinearVelocity();
    
    return glm::vec3(velocity.x(), velocity.y(), velocity.z());
}

void PhysicsComponent::ApplyTorqueToActor(const glm::vec3& torque)
{
    rigid_body()->applyTorque(btVector3(torque.x, torque.y, torque.z));
}