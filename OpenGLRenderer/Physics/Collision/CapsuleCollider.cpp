#include "CapsuleCollider.h"

CapsuleCollider::CapsuleCollider() 
	: m_radius(1.f), m_halfHeight(1.f)
{
	m_type = ColliderType::Capsule;
}

CapsuleCollider::CapsuleCollider(float radius, float height)
	: m_radius(1.f), m_halfHeight(1.f)
{
	m_type = ColliderType::Capsule;
}

CapsuleCollider::CapsuleCollider(float radius, float height, Transform* transform)
	: m_radius(1.f), m_halfHeight(1.f)
{
	m_parentTransform = transform;
	m_type = ColliderType::Capsule;
}

ColliderType CapsuleCollider::getType() const
{
	return m_type;
}

void CapsuleCollider::attachParentTransform(Transform* transform)
{
	m_parentTransform = transform;
}

AABB CapsuleCollider::getAABB() const
{
	return m_colliderAABB;
}

glm::vec3 CapsuleCollider::findFurthestPoint(const glm::vec3& direction) const
{
	glm::vec3 p0 = getTopPoint();
	glm::vec3 p1 = getBottomPoint();

	glm::vec3 furthestPoint = glm::dot(p0, direction) > glm::dot(p1, direction) ? p0 : p1;
	
	return furthestPoint + glm::normalize(direction) * m_radius;
}

glm::vec3 CapsuleCollider::getTopPoint() const 
{
	return m_parentTransform->getPosition() + m_parentTransform->getOrientationQuaternion() * glm::vec3(0.f, 1.f, 0.f) * m_halfHeight;
}

glm::vec3 CapsuleCollider::getBottomPoint() const
{
	return m_parentTransform->getPosition() - m_parentTransform->getOrientationQuaternion() * glm::vec3(0.f, 1.f, 0.f) * m_halfHeight;
}

glm::vec3 CapsuleCollider::getPosition() const 
{
	return m_parentTransform->getPosition();
}

glm::quat CapsuleCollider::getOrientation() const
{
	return m_parentTransform->getOrientationQuaternion();
}

float CapsuleCollider::getRadius() const 
{
	return m_radius;
}
float CapsuleCollider::getHalfHeight() const 
{
	return m_halfHeight;
}

