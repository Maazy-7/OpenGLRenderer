#include "Physics/Collision/Collider.h"

ColliderType Collider::getType() const
{
	return m_type;
}

Transform* Collider::getTransform() const 
{
	return m_parentTransform;
}

Rigidbody* Collider::getRigidbody() const 
{
	return m_parentRigidBody;
}

glm::vec3 Collider::getPosition() const 
{
	return m_parentTransform->getPosition();
}

glm::quat Collider::getOrientation() const
{
	return m_parentTransform->getOrientationQuaternion();
}

AABB Collider::getAABB() const 
{
	return m_colliderAABB;
}

void Collider::attachParentTransform(Transform* transform)
{
	m_parentTransform = transform;
}

void Collider::attachParentRigidBody(Rigidbody* rigidbody)
{
	m_parentRigidBody = rigidbody;
}

bool Collider::isStatic() const 
{
	return m_parentRigidBody->isStatic();
}