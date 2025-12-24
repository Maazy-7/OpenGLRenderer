#pragma once

#include <vector>
#include <memory>

#include "Physics/Components/Rigidbody.h"
#include "Physics/Collision/Collider.h"
#include "Physics/core/DynamicsSolver.h"

class PhysicsEngine
{
	//currently only a singular physics world exists in the physics engine

	std::vector<Rigidbody*> m_Rigidbodies; //list of all bodies the physics engine will operate on
	std::vector<Collider*> m_colliders;
	std::unique_ptr<DynamicsSolver> m_dynamicsSolver;

public:

	PhysicsEngine();
	void addRigidBody(Rigidbody* rigidbody);
	void addCollider(Collider* collider);
	void step(float deltaTime);
};

