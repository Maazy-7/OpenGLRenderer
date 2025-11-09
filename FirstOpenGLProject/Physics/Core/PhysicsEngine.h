#pragma once

#include "Physics/Components/Rigidbody.h"
#include "Physics/core/DynamicsSolver.h"

#include <vector>

class PhysicsEngine
{

	std::vector<Rigidbody*> m_Rigidbodies; //list of all bodies the physics engine will operate on
	DynamicsSolver m_dynamicsSolver;

public:

	PhysicsEngine();
	void step(float deltaTime);

};

