#pragma once

#include <btBulletDynamicsCommon.h>

class BulletPhysics
{
public:
	static BulletPhysics* GetInstance();

	btDiscreteDynamicsWorld* GetDynamicsWorld() const;
private:
	BulletPhysics();
	~BulletPhysics();
	static BulletPhysics* m_instance;

	btBroadphaseInterface* m_pBroadphase;
	btDefaultCollisionConfiguration* m_pCollisionConfiguration;
	btCollisionDispatcher* m_pDispatcher;
	btSequentialImpulseConstraintSolver* m_pSolver;
	btDiscreteDynamicsWorld* m_pDynamicsWorld;
};

