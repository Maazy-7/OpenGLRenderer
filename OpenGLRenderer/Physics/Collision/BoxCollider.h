#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <array>

#include "Physics/Collision/Collider.h"
#include "Physics/Components/Transform.h"

struct Face
{
	std::array<glm::vec3, 4> m_vertices;
	glm::vec3 m_normal;
};

class BoxCollider : public Collider
{
private:

	glm::vec3 m_halfExtents;

public:

	BoxCollider();
	BoxCollider(glm::vec3 halfExtents);
	BoxCollider(glm::vec3 halfExtents, Transform* transform);
	
	glm::vec3 findFurthestPoint(const glm::vec3& direction) const override;
	ColliderType getType() const;
	AABB getAABB() const;

	Face getBestFace(const glm::vec3& normal);

	glm::vec3 getHalfExtents() const;
	glm::vec3 getPosition() const override;
	glm::quat getOrientation() const override;//returns quaternion orientation

	void attachParentTransform(Transform* transform) override;
};

