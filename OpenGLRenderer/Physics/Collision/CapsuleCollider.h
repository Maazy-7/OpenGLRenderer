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
	ColliderType getType() const;
	AABB getAABB() const;

	float getRadius() const;
	float getHalfHeight() const;

	glm::vec3 getTopPoint() const;
	glm::vec3 getBottomPoint() const;
	glm::vec3 getPosition() const override;
	glm::quat getOrientation() const override;//returns a quaternion orientation

	void attachParentTransform(Transform* transform) override;
};