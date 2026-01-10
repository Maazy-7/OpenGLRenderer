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

glm::vec3 CapsuleCollider::findFurthestPoint(const glm::vec3& direction) const
{
	glm::vec3 p0 = getTopPoint();
	glm::vec3 p1 = getBottomPoint();

	glm::vec3 furthestPoint = glm::dot(p0, direction) > glm::dot(p1, direction) ? p0 : p1;
	
	return furthestPoint + glm::normalize(direction) * m_radius;
}

glm::vec3 CapsuleCollider::getDimensionsAsVec3() const 
{
	return glm::vec3(m_radius, 2.f * m_halfHeight, m_radius);
}

void CapsuleCollider::updateAABB() 
{
	glm::vec3 axis = getOrientation() * glm::vec3(0, 1, 0);
	glm::vec3 p0 = getPosition() + m_halfHeight * axis;
	glm::vec3 p1 = getPosition() - m_halfHeight * axis;

	glm::vec3 maxPoint = glm::max(p0,p1);
	glm::vec3 minPoint = glm::min(p0, p1);

	m_colliderAABB.m_max = maxPoint + m_radius;
	m_colliderAABB.m_min = minPoint - m_radius;
}

void CapsuleCollider::setColliderScale(glm::vec3 scale)
{
	//capsule by default points up
	m_halfHeight = 0.5f * scale.y;
	m_radius = glm::max(scale.x, scale.z);
}

glm::vec3 CapsuleCollider::getTopPoint() const 
{
	return m_parentTransform->getPosition() + m_parentTransform->getOrientationQuaternion() * glm::vec3(0.f, 1.f, 0.f) * m_halfHeight;
}

glm::vec3 CapsuleCollider::getBottomPoint() const
{
	return m_parentTransform->getPosition() - m_parentTransform->getOrientationQuaternion() * glm::vec3(0.f, 1.f, 0.f) * m_halfHeight;
}

float CapsuleCollider::getRadius() const 
{
	return m_radius;
}
float CapsuleCollider::getHalfHeight() const 
{
	return m_halfHeight;
}

