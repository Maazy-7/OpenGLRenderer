#pragma once

#include <glm/glm.hpp>
#include <array>

#include "Physics/Components/Transform.h"

enum ColliderType 
{
	Sphere,
	Capsule,
	Box
};

class Collider
{
protected:
	ColliderType m_type;
public:
	virtual glm::vec3 findFurthestPoint(const glm::vec3& direction, const Transform& transform) = 0;
};

