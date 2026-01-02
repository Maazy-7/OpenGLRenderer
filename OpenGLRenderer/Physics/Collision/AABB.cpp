#include "AABB.h"

AABB::AABB(glm::vec3 min, glm::vec3 max) 
	: m_min(min), m_max(max)
{

}

AABB::AABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) 
	: m_min(glm::vec3(minX,minY,minZ)), m_max(glm::vec3(maxX,maxY,maxZ))
{

}

void AABB::updateAABB(glm::vec3 min, glm::vec3 max) 
{
	m_min = min;
	m_max = max;
}

void AABB::updateAABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) 
{
	m_min = glm::vec3(minX, minY, minZ);
	m_max = glm::vec3(maxX, maxY, maxZ);
}

bool AABB::AABBIntersect(const AABB& aabb1, const AABB& aabb2) 
{
	return (aabb1.m_min.x <= aabb2.m_max.x && aabb1.m_max.x >= aabb2.m_min.x) &&
		   (aabb1.m_min.y <= aabb2.m_max.y && aabb1.m_max.y >= aabb2.m_min.y) &&
		   (aabb1.m_min.z <= aabb2.m_max.z && aabb1.m_max.z >= aabb2.m_min.z);
}