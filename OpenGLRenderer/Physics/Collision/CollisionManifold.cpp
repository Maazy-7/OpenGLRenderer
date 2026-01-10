#include "Physics/Collision/CollisionManifold.h"

void CollisionManifold::clearManifold() 
{
	rigidbodyA = nullptr;
	rigidbodyB = nullptr;
	m_contactPoints.clear();
	m_normal = glm::vec3(0.f);
	m_depth = 0.f;
}

ManifoldKey::ManifoldKey(uint32_t a, uint32_t b) 
	:aID(a), bID(b)
{
	if (a > b) { std::swap(a,b); }
}

bool ManifoldKey::operator==(const ManifoldKey& other) const 
{
	return aID == other.aID && bID == other.bID;
}