#pragma once

#include <glm/glm.hpp>

#include "Physics/Collision/Collider.h"

class SphereCollider : public Collider
{
	float m_radius;

public:

	SphereCollider();
	SphereCollider(float radius);
	SphereCollider(float radius, Transform* transform);

	glm::vec3 findFurthestPoint(const glm::vec3& direction) const override;
	void updateAABB() override;
	void setColliderScale(glm::vec3 scale) override;
	glm::vec3 getDimensionsAsVec3() const override;

	float getRadius() const;
};