#pragma once

#include <vector>
#include <functional>
#include <algorithm>

#include "util.h"
#include "Physics/Collision/Collider.h"
#include "Physics/Collision/BoxCollider.h"
#include "Physics/Collision/SphereCollider.h"
#include "Physics/Collision/CapsuleCollider.h"
#include "Physics/Collision/CollisionManifold.h"
#include "Physics/Collision/GJK.h"

struct Plane //plane object used for clipping
{
	glm::vec3 m_planeNormal;
	float distance;
};

class CollisionDetectionSystem
{
	std::function<bool(Collider* a, Collider* b, CollisionManifold& collisionManifold)> m_dispatchTable[(int)ColliderType::COUNT][(int)ColliderType::COUNT];

	const float m_contactTolerance = 1e-6f;
	const float m_contactSlop = 0.01f;

	void initCollisionDispatchTable();

	bool boxVsSphere(BoxCollider* a, SphereCollider* b, CollisionManifold& collisionManifold);
	bool boxVsCapsule(BoxCollider* a, CapsuleCollider* b, CollisionManifold& collisionManifold);
	bool boxVsBox(BoxCollider* a, BoxCollider* b, CollisionManifold& collisionManifold);
	bool sphereVsCapsule(SphereCollider* a, CapsuleCollider* b, CollisionManifold& collisionManifold);
	bool sphereVsSphere(SphereCollider* a, SphereCollider* b, CollisionManifold& collisionManifold);
	bool capsuleVsCapsule(CapsuleCollider* a, CapsuleCollider* b, CollisionManifold& collisionManifold);

	void solveBoxVsCapsuleContactPoints(BoxCollider* a, CapsuleCollider* b, CollisionManifold& collisionManifold);
	void solveBoxVsBoxContactPoints(BoxCollider* a, BoxCollider* b, CollisionManifold& collisionManifold);//optomized for specifically Box vs Box

	void clipPolygonAgainstPlane(std::vector<glm::vec3>& polygon, const Plane& plane);
	std::vector<Plane> getFaceSidePlanes(const std::vector<glm::vec3> face, const glm::vec3& normal);
	std::vector<Plane> getOBBFaceSidePlanes(BoxCollider* refCollider, const Face& refFace);//optomized version for boxes

	//finds 2 points on 2 different line segments which represent the shortest distance between the 2 lines
	float getShortestDistanceBetweenTwoLineSegments(glm::vec3 ap0, glm::vec3 ap1, glm::vec3 bp0, glm::vec3 bp1, glm::vec3& outP0, glm::vec3& outP1);

public:

	CollisionDetectionSystem();

	//TODO implement octree data structure for faster collision detection
	void checkCollisions(const std::vector<Collider*>& colliders, std::vector<CollisionManifold>& collisionManifolds);
};

