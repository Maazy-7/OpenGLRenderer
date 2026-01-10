#pragma once

#include <glm/glm.hpp>

#include "Physics/Collision/Collider.h"

class CapsuleCollider : public Collider
{
	float m_radius;
	float m_halfHeight;

public:

	CapsuleCollider();
	CapsuleCollider(float radius, float height);
	CapsuleCollider(float radius, float height, Transform* transform);

	glm::vec3 findFurthestPoint(const glm::vec3& direction) const override;
	void updateAABB() override;
	void setColliderScale(glm::vec3 scale) override;
	glm::vec3 getDimensionsAsVec3() const override;

	float getRadius() const;
	float getHalfHeight() const;

	glm::vec3 getTopPoint() const;
	glm::vec3 getBottomPoint() const;
};