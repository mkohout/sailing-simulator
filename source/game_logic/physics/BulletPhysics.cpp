#include "BulletPhysics.h"

BulletPhysics* BulletPhysics::m_instance;

BulletPhysics* BulletPhysics::GetInstance()
{
	if(!m_instance)
	{
		m_instance = new BulletPhysics();
	}

	return m_instance;
}

btDiscreteDynamicsWorld* BulletPhysics::GetDynamicsWorld() const
{
	return m_pDynamicsWorld;
}

BulletPhysics::BulletPhysics()
{
	m_pBroadphase = new btDbvtBroadphase();
	m_pCollisionConfiguration = new btDefaultCollisionConfiguration();
	m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);
	m_pSolver = new btSequentialImpulseConstraintSolver();
	m_pDynamicsWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pBroadphase, m_pSolver, m_pCollisionConfiguration);
}


BulletPhysics::~BulletPhysics()
{
	delete m_pDynamicsWorld;
	delete m_pSolver;
	delete m_pDispatcher;
	delete m_pCollisionConfiguration;
	delete m_pBroadphase;
}
