#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Physics/Components/Rigidbody.h"

struct ContactPoint 
{
	glm::vec3 position;
	float depth;
	glm::vec3 rA, rB;
	//used for warm starting
	float normalMass;
	float tangentMass[2] = { 0 };
	float normalImpulse = 0;
	float tangentImpulse[2] = { 0 };//friction impulse

	float bias;
};

struct CollisionManifold //manifolds are persistent meaning they will be cached over frames
{
	Rigidbody* rigidbodyA;
	Rigidbody* rigidbodyB;

	glm::vec3 m_normal;//collision normal depth points from A to B
	glm::vec3 m_tangents[2] = { glm::vec3(0.f) };
	float m_depth;//collision depth, seperate from contact depth
	std::vector<ContactPoint> m_contactPoints;
	bool m_updatedInCurrentFrame = true; //considered updated if newly created

	void clearManifold();
};

struct ManifoldKey //key used in map containing the manifolds
{
	uint32_t aID;
	uint32_t bID;

	ManifoldKey(uint32_t a, uint32_t b);
	bool operator==(const ManifoldKey& other) const;
};

struct ManifoldKeyHash //hashing function used for the collision manifold unordered map
{
	size_t operator()(const ManifoldKey& key) const 
	{
		return (size_t)key.aID ^ ((size_t)key.bID << 32);
	}
};