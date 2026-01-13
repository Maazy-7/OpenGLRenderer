#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine()
{

}

void PhysicsEngine::step(float dt) 
{
	applyGravity(dt);
	integrateForces(dt);
	broadPhase();
	narrowPhase();
	solveContacts(dt);
	positionCorrection();
	integrateVelocities(dt);
	updateInertiaTensors();
	updateColliderAABBs();

}

void PhysicsEngine::addRigidBody(Rigidbody* rigidbody) 
{
	m_Rigidbodies.push_back(rigidbody);
}

void PhysicsEngine::addCollider(Collider* collider) 
{
	m_colliders.push_back(collider);
}

void PhysicsEngine::applyGravity(float dt) 
{
	for (Rigidbody* rb : m_Rigidbodies) 
	{	
		if (rb->m_inverseMass == 0.f) { continue; }
		rb->m_velocity += m_gravity * dt;
	}
}

void PhysicsEngine::integrateForces(float dt) 
{

	for (Rigidbody* rb : m_Rigidbodies) 
	{
		if (rb->m_inverseMass == 0.f) { continue; }
		
		rb->m_velocity += rb->m_force * rb->m_inverseMass * dt;
		rb->m_angularVelocity += rb->m_torque * rb->m_invWorldInertia * dt;
	}
}

void PhysicsEngine::broadPhase() 
{
	m_collisionDetectionSystem.findPossibleCollisionPairs(m_colliders, m_possibleContactPairs);
}

void PhysicsEngine::narrowPhase()
{
	m_collisionDetectionSystem.checkCollisions(m_possibleContactPairs, m_collisionManifolds);
	m_possibleContactPairs.clear();
}

void PhysicsEngine::solveContacts(float dt) 
{
	for (auto& [key, manifold] : m_collisionManifolds)
	{
		m_contactSolverSystem.preSolveManifolds(manifold, dt);
	}

	for (size_t i = 0; i < m_contactSolverSystem.getSolverIterationCount(); i++)
	{
		for (auto& [key, manifold] : m_collisionManifolds)
		{
			m_contactSolverSystem.solveManifolds(manifold);
		}
	}
}

void PhysicsEngine::positionCorrection()
{
	for (auto& [key, manifold] : m_collisionManifolds)
	{
		const float slop = 0.01f;
		const float percent = 0.4f;

		float maxDepth = 0.0f;
		for (const ContactPoint& c : manifold.m_contactPoints)
		{
			maxDepth = glm::max(maxDepth, c.depth);
		}

		float correctionMag = glm::max(maxDepth - slop, 0.0f) * percent;
		if (correctionMag <= 0.0f) { continue; }

		float invMassSum = manifold.rigidbodyA->m_inverseMass + manifold.rigidbodyB->m_inverseMass;
		if (invMassSum == 0.0f) { continue; }

		float vn = glm::dot(manifold.m_normal, manifold.rigidbodyB->m_velocity - manifold.rigidbodyA->m_velocity);
		

		glm::vec3 correction = correctionMag * manifold.m_normal;

		manifold.rigidbodyA->m_position -= correction * (manifold.rigidbodyA->m_inverseMass / invMassSum);
		manifold.rigidbodyB->m_position += correction * (manifold.rigidbodyB->m_inverseMass / invMassSum);
		manifold.rigidbodyA->m_parentTransform->translate(manifold.rigidbodyA->m_position);
		manifold.rigidbodyB->m_parentTransform->translate(manifold.rigidbodyB->m_position);

	}
}

void PhysicsEngine::integrateVelocities(float dt) 
{
	for (Rigidbody* rb : m_Rigidbodies) 
	{
		rb->m_position += rb->m_velocity * dt;
		rb->m_angularVelocity *= 0.95f;//damping angular motion
		rb->m_orientation += 0.5f * rb->m_parentTransform->getOrientationQuaternion() * glm::quat(1.f, rb->m_angularVelocity * dt);
		rb->m_orientation = glm::normalize(rb->m_orientation);
		rb->m_parentTransform->updateTransform(rb->m_position, rb->m_orientation);
		rb->m_force = glm::vec3(0.f);
	}
}

void PhysicsEngine::updateColliderAABBs()
{
	for (Collider* collider : m_colliders)
	{
		collider->updateAABB();
	}
}

void PhysicsEngine::updateInertiaTensors()
{
	for (Rigidbody* rb : m_Rigidbodies)
	{
		rb->updateWorldInertia();
	}
}