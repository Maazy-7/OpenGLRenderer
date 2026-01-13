#include "ContactSolverSystem.h"

ContactSolverSystem::ContactSolverSystem() 
{

}

int ContactSolverSystem::getSolverIterationCount() const 
{
	return m_solverIterations;
}

void ContactSolverSystem::preSolveManifolds(CollisionManifold& manifold, float dt) 
{
	Rigidbody* bodyA = manifold.rigidbodyA;
	Rigidbody* bodyB = manifold.rigidbodyB;
	glm::vec3 normal = manifold.m_normal;

	glm::vec3 tangent1;
	//calculating tangent1 by flipping vector components to get a vector parallel to the normal
	if (glm::abs(normal.x) > glm::abs(normal.y))
	{
		tangent1 = glm::normalize(glm::vec3(normal.z, 0.f, -normal.x));
	}
	else
	{
		tangent1 = glm::normalize(glm::vec3(0.f, -normal.z, normal.y));
	}

	glm::vec3 tangent2 = glm::normalize(glm::cross(normal, tangent1));
	manifold.m_tangents[0] = tangent1;
	manifold.m_tangents[1] = tangent2;

	for (ContactPoint& contact : manifold.m_contactPoints) 
	{
		contact.rA = contact.position - bodyA->m_position;
		contact.rB = contact.position - bodyB->m_position;

		glm::vec3 velA = bodyA->getVelocity() + glm::cross(bodyA->m_angularVelocity, contact.rA);
		glm::vec3 velB = bodyB->getVelocity() + glm::cross(bodyB->m_angularVelocity, contact.rB);

		glm::vec3 vRel = velB - velA;//subtraction order aligns with collision normal: A to B -> B - A


		//normal mass
		glm::vec3 rnA = glm::cross(contact.rA, normal);
		glm::vec3 rnB = glm::cross(contact.rB, normal);

		float effectiveMass =
			bodyA->m_inverseMass + bodyB->m_inverseMass +
			glm::dot(rnA, bodyA->m_invWorldInertia * rnA) +
			glm::dot(rnB, bodyB->m_invWorldInertia * rnB);

		contact.normalMass = effectiveMass > 0.f ? 1.f / effectiveMass : 0.f;

		//mass of tangent 1
		glm::vec3 rtA1 = glm::cross(contact.rA, tangent1);
		glm::vec3 rtB1 = glm::cross(contact.rB, tangent1);

		float tangentMass1 =
			bodyA->m_inverseMass + bodyB->m_inverseMass +
			glm::dot(rtA1, bodyA->m_invWorldInertia * rtA1) +
			glm::dot(rtB1, bodyB->m_invWorldInertia * rtB1);

		contact.tangentMass[0] = tangentMass1 > 0.f ? 1.f / tangentMass1 : 0.f;

		//mass of tangent 2
		glm::vec3 rtA2 = glm::cross(contact.rA, tangent2);
		glm::vec3 rtB2 = glm::cross(contact.rB, tangent2);

		float tangentMass2 =
			bodyA->m_inverseMass + bodyB->m_inverseMass +
			glm::dot(rtA2, bodyA->m_invWorldInertia * rtA2) +
			glm::dot(rtB2, bodyB->m_invWorldInertia * rtB2);

		contact.tangentMass[1] = tangentMass2 > 0.f ? 1.f / tangentMass2 : 0.f;

		//bias calculation
		float vn = glm::dot(normal, vRel);
		float e = glm::min(bodyA->m_restitution, bodyB->m_restitution);
		float restitutionBias = vn < -1.f ? -e * vn : 0.f;

		float slop = 0.01f;//should make these member variables
		float beta = 0.2f;
		contact.bias = beta * glm::max(contact.depth - slop, 0.f) / dt + restitutionBias;

		//warm starting contacts, apply previous frame's normal and tangent impulse
		glm::vec3 impulse = normal * contact.normalImpulse + tangent1 * contact.tangentImpulse[0] + tangent2 * contact.tangentImpulse[1];
		bodyA->applyImpulse(-impulse, contact.rA);
		bodyB->applyImpulse(impulse, contact.rB);
	}
}

void ContactSolverSystem::solveManifolds(CollisionManifold& manifold) 
{
	Rigidbody* bodyA = manifold.rigidbodyA;
	Rigidbody* bodyB = manifold.rigidbodyB;
	glm::vec3 normal = manifold.m_normal;
	float mu = glm::sqrt(bodyA->m_frictionCoefficient * bodyB->m_frictionCoefficient);

	for (ContactPoint& contact : manifold.m_contactPoints) 
	{
		//normal constraint
		glm::vec3 velA = bodyA->m_velocity + glm::cross(bodyA->m_angularVelocity, contact.rA);
		glm::vec3 velB = bodyB->m_velocity + glm::cross(bodyB->m_angularVelocity, contact.rB);

		glm::vec3 vRel = velB - velA;
		float vn = glm::dot(vRel, normal);

		float lambda = -(vn + contact.bias) * contact.normalMass;
		float old = contact.normalImpulse;
		contact.normalImpulse = glm::max(old + lambda, 0.f);
		lambda = contact.normalImpulse - old;

		glm::vec3 impulse = lambda * normal;
		bodyA->applyImpulse(-impulse, contact.rA);
		bodyB->applyImpulse(impulse, contact.rB);

		vRel = bodyB->m_velocity + glm::cross(bodyB->m_angularVelocity, contact.rB) - 
			(bodyA->m_velocity + glm::cross(bodyA->m_angularVelocity, contact.rA));
		
		float maxFriction = mu * contact.normalImpulse;
		
		//friction constraints
		for (int i = 0; i < 2; i++)
		{
			float vt = glm::dot(vRel, manifold.m_tangents[i]);
			float lambdaT = -vt * contact.tangentMass[i];

			float oldTangentImpulse = contact.tangentImpulse[i];
			contact.tangentImpulse[i] = glm::clamp(oldTangentImpulse + lambdaT, -maxFriction, maxFriction);
			lambdaT = contact.tangentImpulse[i] - oldTangentImpulse;
			bodyA->applyImpulse(-lambdaT * manifold.m_tangents[i], contact.rA);
			bodyB->applyImpulse(lambdaT * manifold.m_tangents[i], contact.rB);
		}
	}
}