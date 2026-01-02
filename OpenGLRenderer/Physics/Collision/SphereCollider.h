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
	ColliderType getType() const override;
	AABB getAABB() const override;

	float getRadius() const;
	glm::vec3 getPosition() const override;
	glm::quat getOrientation() const override;//returns a quaternion orientation

	void attachParentTransform(Transform* transform) override;
};