#pragma once

#include <vector>
#include <memory>
#include <utility>

#include "Physics/Components/Rigidbody.h"
#include "Physics/Collision/Collider.h"
#include "Physics/core/CollisionDetectionSystem.h"
#include "Physics/core/DynamicsSolver.h"
#include "Physics/Collision/GJK.h"
#include "Physics/Collision/CollisionManifold.h"


class PhysicsEngine
{
	//currently only a singular physics world exists in the physics engine

	std::vector<Rigidbody*> m_Rigidbodies;
	std::vector<Collider*> m_colliders;
	std::vector<std::pair<Collider*, Collider*>> m_contactPairs;
	std::vector<CollisionManifold> m_collisionManifolds;
	
	CollisionDetectionSystem m_collisionDetectionSystem;
	DynamicsSolver m_dynamicsSolver;


public:

	PhysicsEngine();

	void addRigidBody(Rigidbody* rigidbody);
	void addCollider(Collider* collider);
	void step(float deltaTime);

	void updateColliders();
	void checkCollisions();
	void broadPhase();
	void narrowPhase();
	void solveCollisions();
	void integratePositions();
};

