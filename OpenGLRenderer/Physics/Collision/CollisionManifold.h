#pragma once

#include <glm/glm.hpp>
#include <vector>

struct ContactPoint 
{
	float m_depth;
	glm::vec3 position;
	float accumulatedImpulse;//used for warm starting
};

struct CollisionManifold
{
	glm::vec3 m_normal;
	float m_depth;//collision normal depth
	std::vector<ContactPoint> m_contactPoints;

	void clearManifold()
	{
		m_contactPoints.clear();
		m_normal = glm::vec3(0.f);
	}
};