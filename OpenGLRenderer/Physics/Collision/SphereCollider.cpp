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

glm::vec3 SphereCollider::findFurthestPoint(const glm::vec3& direction) const
{
	return m_parentTransform->getPosition() + glm::normalize(direction) * m_radius;
}

glm::vec3 SphereCollider::getDimensionsAsVec3() const 
{
	return glm::vec3(m_radius);
}

void SphereCollider::updateAABB()
{
	m_colliderAABB.m_max = getPosition() + m_radius;
	m_colliderAABB.m_min = getPosition() - m_radius;
}

void SphereCollider::setColliderScale(glm::vec3 scale)
{
	//ensures sphere will fully incapsulate object based of the transform's scale
	m_radius = glm::max(glm::max(scale.x, scale.y), scale.z);//set radius to largest axis of its transform's scale
}

float SphereCollider::getRadius() const 
{
	return m_radius;
}