#pragma once

#include <glm/glm.hpp>

class Collider;
class SphereCollider;
class CapsuleCollider;
class BoxCollider;

class AABB
{
private:
	glm::vec3 m_min;
	glm::vec3 m_max;

public:

	AABB() {};
	AABB(glm::vec3 min, glm::vec3 max);
	AABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);

	void updateAABB(glm::vec3 min, glm::vec3 max);
	void updateAABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);

	static bool AABBIntersect(const AABB& aabb1, const AABB& aabb2);

	friend Collider;
	friend SphereCollider;
	friend CapsuleCollider;
	friend BoxCollider;
};

