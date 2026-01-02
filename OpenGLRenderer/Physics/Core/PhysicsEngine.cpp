#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine()
{

}

void PhysicsEngine::step(float dt) 
{
	m_collisionManifolds.clear();
	m_collisionDetectionSystem.checkCollisions(m_colliders, m_collisionManifolds);
}

void PhysicsEngine::addRigidBody(Rigidbody* rigidbody) 
{
	m_Rigidbodies.push_back(rigidbody);
}

void PhysicsEngine::addCollider(Collider* collider) 
{
	m_colliders.push_back(collider);
}

void PhysicsEngine::checkCollisions() 
{
	
}

void PhysicsEngine::broadPhase() 
{

}

void PhysicsEngine::narrowPhase()
{

}