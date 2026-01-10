#pragma once

#include <vector>
#include <memory>
#include <utility>
#include <unordered_map>

#include "Physics/Components/Rigidbody.h"
#include "Physics/Collision/Collider.h"
#include "Physics/core/CollisionDetectionSystem.h"
#include "Physics/core/ContactSolverSystem.h"
#include "Physics/Collision/GJK.h"
#include "Physics/Collision/CollisionManifold.h"


class PhysicsEngine
{
	//currently only a singular physics world exists in the physics engine

	std::vector<Rigidbody*> m_Rigidbodies;
	std::vector<Collider*> m_colliders;
	std::vector<std::pair<Collider*, Collider*>> m_possibleContactPairs;
	std::unordered_map<ManifoldKey, CollisionManifold, ManifoldKeyHash> m_collisionManifolds;
	
	CollisionDetectionSystem m_collisionDetectionSystem;
	ContactSolverSystem m_contactSolverSystem;

	const glm::vec3 m_gravity = glm::vec3(0.f,-1.f,0.f);

public:

	PhysicsEngine();

	void addRigidBody(Rigidbody* rigidbody);
	void addCollider(Collider* collider);
	void step(float deltaTime);

	void applyGravity(float dt);
	void integrateForces(float dt);
	void broadPhase();
	void narrowPhase();
	void solveContacts(float dt);
	void integrateVelocities(float dt);
	void updateColliderAABBs();
	void updateInertiaTensors();
};

