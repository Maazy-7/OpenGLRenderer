#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine() {}

void PhysicsEngine::step(float dt) 
{

}

void PhysicsEngine::addRigidBody(Rigidbody* rigidbody) 
{
	m_Rigidbodies.push_back(rigidbody);
}

void PhysicsEngine::addCollider(Collider* collider) 
{
	m_colliders.push_back(collider);
}