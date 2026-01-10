#include "CollisionDetectionSystem.h"

CollisionDetectionSystem::CollisionDetectionSystem() 
{
	initCollisionDispatchTable();
}

void CollisionDetectionSystem::initCollisionDispatchTable() 
{
	//Setting up collision dispatch table, 2d array of functions allowing for O(1) selection of the collision method based off the shape type

	//Box vs ...
	m_dispatchTable[(int)ColliderType::Box][(int)ColliderType::Sphere] = [this](Collider* a, Collider* b, CollisionManifold& collisionManifold) 
		{ return boxVsSphere(static_cast<BoxCollider*>(a), static_cast<SphereCollider*>(b), collisionManifold); }; //Box vs Sphere

	m_dispatchTable[(int)ColliderType::Sphere][(int)ColliderType::Box] = [this](Collider* a, Collider* b, CollisionManifold& collisionManifold)
		{ return boxVsSphere(static_cast<BoxCollider*>(b), static_cast<SphereCollider*>(a), collisionManifold); }; //Sphere vs Box
	
	m_dispatchTable[(int)ColliderType::Box][(int)ColliderType::Capsule] = [this](Collider* a, Collider* b, CollisionManifold& collisionManifold)
		{ return boxVsCapsule(static_cast<BoxCollider*>(a), static_cast<CapsuleCollider*>(b), collisionManifold); }; //Box vs Capsule

	m_dispatchTable[(int)ColliderType::Capsule][(int)ColliderType::Box] = [this](Collider* a, Collider* b, CollisionManifold& collisionManifold)
		{ return boxVsCapsule(static_cast<BoxCollider*>(b), static_cast<CapsuleCollider*>(a), collisionManifold); }; //Capsule vs Box

	m_dispatchTable[(int)ColliderType::Box][(int)ColliderType::Box] = [this](Collider* a, Collider* b, CollisionManifold& collisionManifold)
		{ return boxVsBox(static_cast<BoxCollider*>(a), static_cast<BoxCollider*>(b), collisionManifold); }; //Box vs Box
	
	//Sphere vs ...
	m_dispatchTable[(int)ColliderType::Sphere][(int)ColliderType::Sphere] = [this](Collider* a, Collider* b, CollisionManifold& collisionManifold)
		{ return sphereVsSphere(static_cast<SphereCollider*>(a), static_cast<SphereCollider*>(b), collisionManifold); }; //Sphere vs Sphere

	m_dispatchTable[(int)ColliderType::Sphere][(int)ColliderType::Capsule] = [this](Collider* a, Collider* b, CollisionManifold& collisionManifold)
		{ return sphereVsCapsule(static_cast<SphereCollider*>(a), static_cast<CapsuleCollider*>(b), collisionManifold); }; //Sphere vs Capsule

	m_dispatchTable[(int)ColliderType::Capsule][(int)ColliderType::Sphere] = [this](Collider* a, Collider* b, CollisionManifold& collisionManifold)
		{ return sphereVsCapsule(static_cast<SphereCollider*>(b), static_cast<CapsuleCollider*>(a), collisionManifold); }; //Capsule vs Sphere

	//Capsule vs ...
	m_dispatchTable[(int)ColliderType::Capsule][(int)ColliderType::Capsule] = [this](Collider* a, Collider* b, CollisionManifold& collisionManifold)
		{ return capsuleVsCapsule(static_cast<CapsuleCollider*>(a), static_cast<CapsuleCollider*>(b), collisionManifold); }; //Capsule vs Capsule
}

//broadphase collision detection
void CollisionDetectionSystem::findPossibleCollisionPairs(const std::vector<Collider*>& colliders, std::vector<std::pair<Collider*, Collider*>>& collisionPairs)
{
	//collision check is currently O(N^2) with an AABB broadphase
	//however soon and octree will be implmented to further improve the collision detection algorithm
	for (int i = 0; i < colliders.size(); i++) 
	{
		Collider* colliderA = colliders[i];
		for (int j = i + 1; j < colliders.size(); j++) 
		{
			Collider* colliderB = colliders[j];
			if (AABB::AABBIntersect(colliderA->getAABB(), colliderB->getAABB()) ) 
			{
				collisionPairs.push_back(std::make_pair(colliderA,colliderB));
			}
		}
	}
}


//narrow phase collision detection on possible collision pairs from broadphase
void CollisionDetectionSystem::checkCollisions(const std::vector<std::pair<Collider*, Collider*>>& collisionPairs, std::unordered_map<ManifoldKey, CollisionManifold, ManifoldKeyHash>& collisionManifolds)
{
	CollisionManifold tempManifold;
	for (size_t i = 0; i < collisionPairs.size(); i++)
	{
		Collider* colliderA = collisionPairs[i].first;
		Collider* colliderB = collisionPairs[i].second;

		if (colliderA == colliderB) { continue; }//skip identical colliders
		//collision function
		auto collisionFunc = m_dispatchTable[(int)colliderA->getType()][(int)colliderB->getType()];

		if (!collisionFunc) { std::cout << "Error selecting collision function\n"; continue; }//shouldn't happen

		if (collisionFunc(colliderA, colliderB, tempManifold)) //collision has occured
		{
			ManifoldKey key(colliderA->getRigidbody()->getID(), colliderB->getRigidbody()->getID());

			auto it = collisionManifolds.find(key);

			if (it != collisionManifolds.end())//manifold for the current pair exist, so try and match its contacts 
			{
				matchContactPoints(tempManifold, it->second);
			}

			tempManifold.m_updatedInCurrentFrame = true;

			collisionManifolds[key] = tempManifold;//if manifold for current pair doesn't exist then new one gets created
			tempManifold.clearManifold();//emptying contact points and reseting normal and depth to 0

		}
	}
	//clearing manifolds that weren't updated in the current frame as those pair aren't in collision anymore
	for (auto it = collisionManifolds.begin(); it != collisionManifolds.end();) 
	{
		if (!it->second.m_updatedInCurrentFrame) //removing manifold
		{
			it = collisionManifolds.erase(it);
		}
		else //keeping manifold
		{
			it->second.m_updatedInCurrentFrame = false;
			++it;
		}
	}
}

void CollisionDetectionSystem::matchContactPoints(CollisionManifold& newManifold, const CollisionManifold& oldManifold) 
{
	//currently uses position matching to match 2 contact points
	
	for (ContactPoint& newContact : newManifold.m_contactPoints) 
	{
		const ContactPoint* bestContact = nullptr;//this contact if it exists will match with the current contact
		float bestDistSqr = m_persistentContactThreshold * m_persistentContactThreshold;
		for (const ContactPoint& oldContact : oldManifold.m_contactPoints) 
		{
			float distSqr = glm::dot(newContact.position - oldContact.position, newContact.position - oldContact.position);
			if (distSqr < bestDistSqr)
			{
				bestDistSqr = distSqr;
				bestContact = &oldContact;
			}
		}
		//set new contact's impulses to the old ones to warm start the contacts
		if (bestContact) 
		{
			newContact.normalImpulse = bestContact->normalImpulse;
			newContact.tangentImpulse[0] = bestContact->tangentImpulse[0];
			newContact.tangentImpulse[1] = bestContact->tangentImpulse[1];
		}
	}
}

bool CollisionDetectionSystem::boxVsSphere(BoxCollider* a, SphereCollider* b, CollisionManifold& collisionManifold)
{
	glm::vec3 sphereWorldPos = b->getPosition();
	glm::vec3 halfExtents = a->getHalfExtents();

	//convert sphere to box's local space
	glm::vec3 sphereLocalPos = glm::conjugate(a->getOrientation()) * (sphereWorldPos - a->getPosition());

	//get closest point from box to sphere, at this point this is essentially an AABB vs sphere check
	glm::vec3 closestPointLocal;
	closestPointLocal.x = glm::clamp(sphereLocalPos.x, -halfExtents.x, halfExtents.x);
	closestPointLocal.y = glm::clamp(sphereLocalPos.y, -halfExtents.y, halfExtents.y);
	closestPointLocal.z = glm::clamp(sphereLocalPos.z, -halfExtents.z, halfExtents.z);

	glm::vec3 delta = sphereLocalPos - closestPointLocal;
	float distSqr = glm::dot(delta, delta);
	float radius = b->getRadius();
	
	if (distSqr > radius * radius) 
	{
		return false;
	}

	float distance = glm::sqrt(distSqr);
	
	//transforming contact point to world space
	glm::vec3 closestPointWorld = a->getPosition() + a->getOrientation() * closestPointLocal;
	float depth = radius;//default only when degenerate case occurs

	if (distance > 1e-6f) //sphere's center is outside the box's volume
	{
		depth = radius - distance;
		collisionManifold.m_normal = (sphereWorldPos - closestPointWorld) / distance;
	}
	else //degenerate case where sphere is inside box
	{
		//TODO handle case where sphere is inside the box
		std::cout << "Box vs Sphere degenerate case occured\n";
	}

	collisionManifold.m_contactPoints.push_back({closestPointWorld, depth});
	collisionManifold.rigidbodyA = a->getRigidbody();
	collisionManifold.rigidbodyB = b->getRigidbody();
	return true;
}

bool CollisionDetectionSystem::boxVsCapsule(BoxCollider* a, CapsuleCollider* b, CollisionManifold& collisionManifold)
{
	auto [simplex, collide] = GJK(a, b);
	if (collide) 
	{
		CollisionManifold temp = EPA(simplex, a, b);
		collisionManifold.m_normal = temp.m_normal;
		collisionManifold.rigidbodyA = a->getRigidbody();
		collisionManifold.rigidbodyB = b->getRigidbody();
		//solveBoxVsCapsuleContactPoints(a, b, collisionManifold);
		return true;
	}
	return false;
}

bool CollisionDetectionSystem::boxVsBox(BoxCollider* a, BoxCollider* b, CollisionManifold& collisionManifold)
{
	auto [simplex, collide] = GJK(a, b);
	if (collide)
	{
		CollisionManifold temp = EPA(simplex, a, b);//normal from A to B
		collisionManifold.m_normal = temp.m_normal;
		collisionManifold.rigidbodyA = a->getRigidbody();
		collisionManifold.rigidbodyB = b->getRigidbody();
		solveBoxVsBoxContactPoints(a, b, collisionManifold);
		return true;
	}
	return false;
}

bool CollisionDetectionSystem::sphereVsCapsule(SphereCollider* a, CapsuleCollider* b, CollisionManifold& collisionManifold)
{
	glm::vec3 p0 = b->getTopPoint();
	glm::vec3 p1 = b->getBottomPoint();
	glm::vec3 spherePos = a->getPosition();
	
	glm::vec3 capsuleSegment = p1 - p0;

	//parametric t position on the capsule's line segment
	float t = glm::clamp(glm::dot(capsuleSegment, spherePos - p0) / glm::dot(capsuleSegment, capsuleSegment), 0.f, 1.f);

	glm::vec3 closestPointOnSegment = p0 + t * capsuleSegment;

	//essentially the sphere vs sphere algorithm from here on
	glm::vec3 direction = spherePos - closestPointOnSegment;
	float distSqr = glm::dot(direction, direction);
	float radiiSum = a->getRadius() + b->getRadius();
	
	if (distSqr > radiiSum*radiiSum) 
	{
		return false;
	}

	float distance = glm::length(direction);
	float depth = radiiSum - distance;
	collisionManifold.m_normal = distance > 0.f ? direction / distance : glm::vec3(0.f, 1.f, 0.f);
	glm::vec3 contactPoint = spherePos - collisionManifold.m_normal * a->getRadius();
	collisionManifold.m_contactPoints.push_back({contactPoint, depth});
	collisionManifold.rigidbodyA = a->getRigidbody();
	collisionManifold.rigidbodyB = b->getRigidbody();
	return true;
}

bool CollisionDetectionSystem::sphereVsSphere(SphereCollider* a, SphereCollider* b, CollisionManifold& collisionManifold)
{
	float r1 = a->getRadius();
	float r2 = b->getRadius();
	glm::vec3 direction = b->getPosition() - a->getPosition();

	if (glm::dot(direction, direction) > (r1 + r2) * (r1 + r2)) 
	{
		return false;
	}
	
	float distance = glm::length(direction);
	float depth = r1 + r2 - distance;
	collisionManifold.m_normal = distance > 0.f ? direction / distance : glm::vec3(0.f, 1.f, 0.f);
	//when 2 spheres collide only a singular contact point exists
	glm::vec3 contactPoint = a->getPosition() + collisionManifold.m_normal * a->getRadius();
	collisionManifold.m_contactPoints.push_back({contactPoint, depth});
	collisionManifold.rigidbodyA = a->getRigidbody();
	collisionManifold.rigidbodyB = b->getRigidbody();
	return true;
}

bool CollisionDetectionSystem::capsuleVsCapsule(CapsuleCollider* a, CapsuleCollider* b, CollisionManifold& collisionManifold)
{
	glm::vec3 p0a = a->getTopPoint();
	glm::vec3 p1a = a->getBottomPoint();
	glm::vec3 p0b = b->getTopPoint();
	glm::vec3 p1b = b->getBottomPoint();

	glm::vec3 point0, point1;

	float distSqr = getShortestDistanceBetweenTwoLineSegments(p0a, p1a, p0b, p1b, point0, point1);

	//essentially a sphere vs sphere check from here on
	float radiiSum = a->getRadius() + b->getRadius();
	if (distSqr > radiiSum * radiiSum) 
	{
		return false;
	}

	float distance = glm::sqrt(distSqr);
	collisionManifold.m_normal = distance > 1e-6f ? (point0 - point1) / distance : glm::vec3(0.f, 1.f, 0.f);
	float depth = radiiSum - distance;

	//only 1 contact point will be generated for capsule vs capsule, this works assuming persistent manifolds are implemented
	glm::vec3 contact1 = point0 - collisionManifold.m_normal * a->getRadius();
	glm::vec3 contact2 = point1 + collisionManifold.m_normal * b->getRadius();
	glm::vec3 contactPoint = (point0 + point1) * 0.5f;
	collisionManifold.m_contactPoints.push_back({ contactPoint, depth});
	collisionManifold.rigidbodyA = a->getRigidbody();
	collisionManifold.rigidbodyB = b->getRigidbody();
	return true;
}

float CollisionDetectionSystem::getShortestDistanceBetweenTwoLineSegments(glm::vec3 p0a, glm::vec3 p1a, glm::vec3 p0b, glm::vec3 p1b, glm::vec3& outP0, glm::vec3& outP1) 
{
	glm::vec3 seg1 = p1a - p0a;
	glm::vec3 seg2 = p1b - p0b;
	glm::vec3 seg2ToSeg1 = p0a - p0b;//line from seg2 top point to seg1 top point

	float distSqrSeg1 = glm::dot(seg1, seg1);
	float distSqrSeg2 = glm::dot(seg2, seg2);
	float f = glm::dot(seg2, seg2ToSeg1);

	float s, t;//parameteic values representing the closest position on line 1 and 2 respectively

	if (distSqrSeg1 <= 1e-6f && distSqrSeg2 <= 1e-6) //both line segments are actually points - degenerate case
	{
		s = t = 0;
		outP0 = seg1;
		outP1 = seg2;
		return glm::dot(outP1 - outP0, outP1 - outP0); //squared distance between the 2 closest points
	}
	if (distSqrSeg1 <= 1e-6f) //line segment 1 is a point - degenerate case
	{
		s = 0;
		t = glm::clamp(f / distSqrSeg2, 0.f, 1.f);
	}
	else 
	{
		float c = glm::dot(seg1, seg2ToSeg1);
		if (distSqrSeg2 <= 1e-6f) //line segment 2 is a point - degenerate case
		{
			t = 0;
			s = glm::clamp(-c / distSqrSeg1, 0.f, 1.f);
		}
		else //neither of the 2 lines are points and are proper line segments
		{
			float b = glm::dot(seg1,seg2);
			float denom = distSqrSeg1 * distSqrSeg2 - b * b; //determines if the 2 lines are paralell

			if (denom != 0.f) { s = glm::clamp((b * f - c * distSqrSeg2) / denom, 0.f, 1.f); }//lines aren't parallel
			else { s = 0.f; }

			t = (b * s + f) / distSqrSeg2;

			if (t < 0.0f) 
			{
				t = 0.f;
				s = glm::clamp(-c / distSqrSeg1, 0.f, 1.f);
			}
			else if (t > 1.f) 
			{
				t = 1.f;
				s = glm::clamp((b - c) / distSqrSeg1, 0.f, 1.f);
			}
		}
	}
	outP0 = p0a + seg1 * s;//position of first point along the line segement
	outP1 = p0b + seg2 * t;//position of second point along the line segment
	return glm::dot(outP1 - outP0, outP1 - outP0);//squared distance between the 2 points
}

void CollisionDetectionSystem::solveBoxVsBoxContactPoints(BoxCollider* a, BoxCollider* b, CollisionManifold& collisionManifold) 
{
	//To find the contact points for box vs box (and meshes) a one shot contact generation algorithm using Sutherland-Hodgman clipping will be used

	//First the reference and incident faces need to be found
	glm::vec3 collisionNormal = collisionManifold.m_normal;// from A to B

	Face faceA = a->getBestFace(collisionNormal);
	Face faceB = b->getBestFace(-collisionNormal);

	float alignmentFaceA = glm::dot(faceA.m_normal, collisionNormal);
	float alignmentFaceB = glm::dot(faceB.m_normal, -collisionNormal);

	Face referenceFace;
	Face incidentFace;
	glm::vec3 clipNormal;
	std::vector<Plane> referenceFaceSidePlanes;//incident face will be clipped by these planes

	if (alignmentFaceA >= alignmentFaceB) //face A is the reference face 
	{
		referenceFace = faceA;
		incidentFace = faceB;
		clipNormal = collisionNormal;
		referenceFaceSidePlanes = getOBBFaceSidePlanes(a, referenceFace);
	}
	else //face B is the reference face
	{
		referenceFace = faceB;
		incidentFace = faceA;
		clipNormal = -collisionNormal;
		referenceFaceSidePlanes = getOBBFaceSidePlanes(b, referenceFace);
	}

	//polygon that will get clipped against the reference face's side planes, will start as the incident face
	std::vector<glm::vec3> clipPolygon = std::vector(std::begin(incidentFace.m_vertices), std::end(incidentFace.m_vertices));//Face struct for box collider stores only 4 vertices

	for (Plane& plane : referenceFaceSidePlanes) 
	{
		clipPolygonAgainstPlane(clipPolygon, plane);
		if (clipPolygon.empty()) { break; }
	}

	//clippedPolygon now stores all possible contact points
	//however only the points behind the refernce face will be considered actual contact points
	if (!clipPolygon.empty())
	{
		for (glm::vec3& contactPoint : clipPolygon)
		{
			Plane plane{clipNormal, glm::dot(clipNormal, referenceFace.m_vertices[0])};
			float distance = glm::dot(clipNormal, contactPoint) - plane.distance;
			if (distance <= m_contactSlop)
			{
				collisionManifold.m_contactPoints.push_back({contactPoint, -distance});
			}
		}
	}

	if (collisionManifold.m_contactPoints.size() > 4)//reducing number of contact points to 4
	{
		//since these are box vs box contact points, the 4 contacts with the largest depth are sufficient
		std::sort(collisionManifold.m_contactPoints.begin(), collisionManifold.m_contactPoints.end(),
			[](const ContactPoint& p1, const ContactPoint& p2) { return p1.depth > p2.depth; });//sorting in descending order

		//since list is now sorted in descending order the first 4 contacts will be the maximum
		collisionManifold.m_contactPoints = std::vector(collisionManifold.m_contactPoints.begin(), collisionManifold.m_contactPoints.begin() + 4);
	}
	if (collisionManifold.m_contactPoints.empty())// ideally this shouldn't happen 
	{
		std::cout << "No contact points found\n";
	}
}

void CollisionDetectionSystem::clipPolygonAgainstPlane(std::vector<glm::vec3>& polygon, const Plane& plane) //Sutherland Hodgman clipping Algorithm
{
	if (polygon.empty()) { return; }
	std::vector<glm::vec3> out;
	int size = polygon.size();
	for (int i = 0; i < size; i++) 
	{
		glm::vec3 a = polygon[i];
		glm::vec3 b = polygon[(i + 1) % size];
		//if dot product with normal and vector from plane to point is less than 0 than point is indside the plane otherwise its on the outside
		float da = glm::dot(plane.m_planeNormal, a) - plane.distance;//equivalent to dot( normal, a - pointOnPlane )
		float db = glm::dot(plane.m_planeNormal, b) - plane.distance;
		bool aInside = da <= m_contactTolerance;
		bool bInside = db <= m_contactTolerance;

		if (aInside && bInside) // both points inside plane
		{
			out.push_back(b);
		}
		else if (aInside && !bInside) //edge leaving plane, add point intersecting with plane
		{
			float t = da / (da - db);
			out.push_back(a + t*(b-a));
		}
		else if (!aInside && bInside) //edge entering the plane, add point intersecting the plane and the end of the edge
		{
			float t = da / (da - db);
			out.push_back(a + t*(b-a));
			out.push_back(b);
		}
		//if no points are inside the plane then don't add any points
	}
	polygon = out;
}

std::vector<Plane> CollisionDetectionSystem::getOBBFaceSidePlanes(BoxCollider* refCollider, const Face& refFace) //optomized version for boxes
{
	glm::quat orientation = refCollider->getOrientation();
	glm::vec3 halfExtents = refCollider->getHalfExtents();
	glm::vec3 colliderPos = refCollider->getPosition();


	glm::vec3 worldsAxis[3];
	worldsAxis[0] = orientation * glm::vec3(1.f, 0.f, 0.f);
	worldsAxis[1] = orientation * glm::vec3(0.f, 1.f, 0.f);
	worldsAxis[2] = orientation * glm::vec3(0.f, 0.f, 1.f);

	//need to get the axis that is most aligned with the reference face's normal
	int normalAxisIndex = 0;//indexes the world space axis list
	float maxDot = -1.f;
	for (int i = 0; i < 3; i++) 
	{
		float d = glm::abs(glm::dot(worldsAxis[i], refFace.m_normal));
		if (d > maxDot) { maxDot = d; normalAxisIndex = i; }
	}

	//the other 2 axis represent the side planes
	int axis1Index = (normalAxisIndex + 1) % 3;
	int axis2Index = (normalAxisIndex + 2) % 3;
	
	std::vector<Plane> sidePlanes;

	//the 4 side planes
	sidePlanes.push_back({ worldsAxis[axis1Index], glm::dot(worldsAxis[axis1Index] , colliderPos + worldsAxis[axis1Index] * halfExtents[axis1Index])});
	sidePlanes.push_back({ -worldsAxis[axis1Index], glm::dot(-worldsAxis[axis1Index] , colliderPos - worldsAxis[axis1Index] * halfExtents[axis1Index]) });
	sidePlanes.push_back({ worldsAxis[axis2Index], glm::dot(worldsAxis[axis2Index] , colliderPos + worldsAxis[axis2Index] * halfExtents[axis2Index]) });
	sidePlanes.push_back({ -worldsAxis[axis2Index], glm::dot(-worldsAxis[axis2Index] , colliderPos - worldsAxis[axis2Index] * halfExtents[axis2Index]) });
	return sidePlanes;
}