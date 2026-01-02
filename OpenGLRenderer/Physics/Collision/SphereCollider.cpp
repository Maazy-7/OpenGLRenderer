#include "SphereCollider.h"

SphereCollider::SphereCollider()
	:m_radius(1.f)
{
	m_type = ColliderType::Sphere;
}

SphereCollider::SphereCollider(float radius) 
	:m_radius(radius)
{
	m_type = ColliderType::Sphere;
}

SphereCollider::SphereCollider(float radius, Transform* transform) 
	:m_radius(radius)
{
	m_parentTransform = transform;
	m_type = ColliderType::Sphere;
}

ColliderType SphereCollider::getType() const
{
	return m_type;
}

void SphereCollider::attachParentTransform(Transform* transform) 
{
	m_parentTransform = transform;
}

glm::vec3 SphereCollider::findFurthestPoint(const glm::vec3& direction) const
{
	return m_parentTransform->getPosition() + glm::normalize(direction) * m_radius;
}

AABB SphereCollider::getAABB() const 
{
	return m_colliderAABB;
}

float SphereCollider::getRadius() const 
{
	return m_radius;
}

glm::vec3 SphereCollider::getPosition() const 
{
	return m_parentTransform->getPosition();
}

glm::quat SphereCollider::getOrientation() const 
{
	return m_parentTransform->getOrientationQuaternion();
}