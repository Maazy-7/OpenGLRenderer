#pragma once

#include <glm/glm.hpp>
#include <array>

#include "Physics/Components/Transform.h"


class Collider
{
public:
	virtual glm::vec3 findFurthestPoint(const glm::vec3& direction) = 0;
	virtual void updateCollider(Transform transform) = 0;
	virtual glm::vec3 getPosition() = 0;
	virtual glm::quat getOrientation() = 0;
	virtual glm::vec3 getScale() = 0;
};

