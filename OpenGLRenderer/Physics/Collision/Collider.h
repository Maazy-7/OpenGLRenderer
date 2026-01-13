#pragma once

#include <glm/glm.hpp>
#include <array>

#include "Physics/Components/Transform.h"
#include "Physics/Components/Rigidbody.h"
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
	Rigidbody* m_parentRigidBody;//the rigidbody (if the gameObject has one) that the transform operates on
	AABB m_colliderAABB;//axis aligned bounding box which encapsulates the collider

public:
	virtual glm::vec3 findFurthestPoint(const glm::vec3& direction) const = 0;
	virtual void updateAABB() = 0;//updates AABB min and max so that it fully encapsulates the inner collider
	virtual void setColliderScale(glm::vec3 scale) = 0;//scales collider geometry (radius, height, half extents) based off transform scale
	virtual glm::vec3 getDimensionsAsVec3() const = 0;
	
	ColliderType getType() const;
	AABB getAABB() const;
	Transform* getTransform() const;//should make these private and only accessible by friend classes
	Rigidbody* getRigidbody() const;
	glm::vec3 getPosition() const;
	glm::quat getOrientation() const;//returns a quaternion orientation
	void attachParentTransform(Transform* transform);
	void attachParentRigidBody(Rigidbody* rigidBody);
	bool isStatic() const;

	//temp
	glm::vec3 getAABBmax() const { return m_colliderAABB.m_max; }
	glm::vec3 getAABBmin() const { return m_colliderAABB.m_min; }
};

