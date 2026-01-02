#pragma once

#include <glm/glm.hpp>
#include <array>

#include "Physics/Components/Transform.h"
#include "Physics/Collision/AABB.h"

enum class ColliderType
{
	Sphere,
	Capsule,
	Box,
	COUNT //stores amount of enums in class
};

class Collider
{
protected:
	ColliderType m_type;
	Transform* m_parentTransform;//the transform which this collider belongs to
	AABB m_colliderAABB;//axis aligned bounding box which encapsulates the collider

public:
	virtual glm::vec3 findFurthestPoint(const glm::vec3& direction) const = 0;
	virtual ColliderType getType() const = 0;
	virtual AABB getAABB() const = 0;
	virtual glm::vec3 getPosition() const = 0;
	virtual glm::quat getOrientation() const = 0;//returns a quaternion orientation
	virtual void attachParentTransform(Transform* transform) = 0;
};

