#include "Physics/Collision/GJK.h"


Simplex::Simplex()
	: m_points(), m_size(0)
{
}

Simplex& Simplex::operator=(std::initializer_list<glm::vec3> list) 
{
	for (auto vec = list.begin(); vec != list.end(); vec++) 
	{
		m_points[std::distance(list.begin(), vec)] = *vec;
	}
	m_size = list.size();

	return *this;
}

glm::vec3& Simplex::operator[](unsigned i)
{
	return m_points[i];
}

void Simplex::pushToFront(glm::vec3 point) 
{
	m_points = { point, m_points[0], m_points[1], m_points[2] };
	m_size = std::min<size_t>(m_size+1, 4);
}

size_t Simplex::size() { return m_size; }

auto Simplex::begin() { return m_points.begin(); }
auto Simplex::end() { return m_points.end(); }

glm::vec3 support(CubeCollider* colliderA, CubeCollider* colliderB, const glm::vec3& direction) 
{
	return colliderA->findFurthestPoint(direction) - colliderB->findFurthestPoint(-direction);
}

glm::vec3 support(Collider* colliderA, Transform* transformA, Collider* colliderB, Transform* transformB, const glm::vec3& direction)
{
	return colliderA->findFurthestPoint(direction, *transformA) - colliderB->findFurthestPoint(-direction, *transformB);
}

bool sameDirection(const glm::vec3& a, const glm::vec3& b) 
{
	return glm::dot(a,b) > 0;
}




std::pair<Simplex, bool> GJK(CubeCollider* colliderA, CubeCollider* colliderB)
{
	glm::vec3 supportPoint = support(colliderA, colliderB, colliderA->getPosition() - colliderB->getPosition()); //inital direction
	Simplex simplex;

	simplex.pushToFront(supportPoint);

	glm::vec3 direction = -supportPoint;

	size_t iterations = 0;
	
	while (iterations++ < GJK_EPA_MAX_ITERATIONS)
	{
		supportPoint = support(colliderA, colliderB, direction);

		//if new direction and support point dont have the same direction then shapes cant possible be colliding as a new support point past the origin does not exist
		
		if (glm::dot(supportPoint, direction) <= 0)
		{
			break;
		}

		simplex.pushToFront(supportPoint);

		if (nextSimplex(simplex, direction)) 
		{
			return {simplex, true};
		}
	}
	return { {}, false };
}

std::pair<Simplex, bool> GJK(Collider* colliderA, Transform* transformA, Collider* colliderB, Transform* transformB)
{
	glm::vec3 supportPoint = support(colliderA, transformA, colliderB, transformB, transformB->getPosition() - transformA->getPosition()); //inital direction
	Simplex simplex;

	simplex.pushToFront(supportPoint);

	glm::vec3 direction = -supportPoint;

	size_t iterations = 0;

	while (iterations++ < GJK_EPA_MAX_ITERATIONS)
	{
		supportPoint = support(colliderA, transformA, colliderB, transformB, direction);

		//if new direction and support point dont have the same direction then shapes cant possible be colliding as a new support point past the origin does not exist

		if (glm::dot(supportPoint, direction) <= 0)
		{
			break;
		}

		simplex.pushToFront(supportPoint);

		if (nextSimplex(simplex, direction))
		{
			return { simplex, true };
		}
	}
	return { {}, false };
}

bool nextSimplex(Simplex& simplex, glm::vec3& direction) 
{
	//the goal is to encapsulate the origin in a simplex which if we are able to then a collision has been detected
	//these functions attempt to build a simplex by testing the differernt supporting points calculated
	//eliminating ones that go away from the origin and keep/add ones that are might encapsulate the origin
	switch (simplex.size())
	{
		case 2: return lineCase(simplex, direction);
		case 3: return triangleCase(simplex, direction);
		case 4: return tetrahedronCase(simplex, direction);
	}

	return false;//something bad happened
}

bool lineCase(Simplex& simplex, glm::vec3& direction) 
{
	glm::vec3 a = simplex[0];
	glm::vec3 b = simplex[1];

	glm::vec3 ab = b - a;//line from ab
	glm::vec3 ao = -a;//a is directly away from origin so -a is flipped and directly towards origin

	if (sameDirection(ab, ao)) //if new point is in direction of origin then continue building simplex with current points
	{
		direction = glm::cross(glm::cross(ab,ao), ab);//new direction perpendicular to ab edge
	}
	else //else remove the first point, keep point a and look for a new 2nd point in direction of ao
	{
		simplex = { a };
		direction = ao;
	}

	return false;
}

bool triangleCase(Simplex& simplex, glm::vec3& direction) 
{
	glm::vec3 a = simplex[0];
	glm::vec3 b = simplex[1];
	glm::vec3 c = simplex[2];

	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 ao = -a;

	glm::vec3 abc = glm::cross(ab,ac);//direction of abc face (vector perpendicualr to the abc face)

	//   C
	//   |\
	//   | \
	//   |  \
	//   |   \
	//   |    \ B
	//   |    /
	//   |   /
	//   |  /
	//   | /
	//   |/
	//   A     

	if (sameDirection(glm::cross(abc,ac),ao)) //checking ac side for origin
	{
		if (sameDirection(ac,ao))// if origin and ac in same direction remove point b and find a new point perpendicular to the ac face
		{
			simplex = { a, c };
			direction = glm::cross(glm::cross(ac,ao),ac);
		}
		else //if not then remove c from simplex and check a new line case using a and b
		{
			return lineCase(simplex = { a, b }, direction);
		}
	}
	else  
	{
		if (sameDirection(glm::cross(ab,abc),ao)) //checking ab side for origin
		{
			return lineCase(simplex = { a, b }, direction);
		}
		else //if we've reached here then origin is withing the 2d triangle, next check is to see if origin is above or below the current simplex
		{
			if (sameDirection(abc, ao)) //origin above simplex, new direction is above triangle towards origin
			{
				direction = abc;
			}
			else //origin is below triangle, new direction is below triangle towards origin
			{
				simplex = { a, c, b };//reversing winding order - so cross product of edges points downwards
				direction = -abc;
			}
		}
	}

	return false;
}

bool tetrahedronCase(Simplex& simplex, glm::vec3& direction) 
{
	glm::vec3 a = simplex[0];
	glm::vec3 b = simplex[1];
	glm::vec3 c = simplex[2];
	glm::vec3 d = simplex[3];

	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 ad = d - a;
	glm::vec3 ao = -a;

	glm::vec3 abc = glm::cross(ab, ac);//abc face
	glm::vec3 acd = glm::cross(ac, ad);//adc face
	glm::vec3 adb = glm::cross(ad, ab);//adb face

	//if origin in direction of certain face other point behind face gets removed and new triangle is tested with direction pointing towards origin
	if (sameDirection(abc, ao)) 
	{
		return triangleCase(simplex = { a, b, c }, direction);
	}
	if (sameDirection(acd, ao))
	{
		return triangleCase(simplex = { a, c, d }, direction);
	}
	if (sameDirection(adb, ao))
	{
		return triangleCase(simplex = { a, d, b }, direction);
	}

	//if all faces point away from origin then the simplex encapsulates the origin and a collision has occured
	return true;
}


CollisionManifold EPA(Simplex simplex, CubeCollider* colliderA, CubeCollider* colliderB)
{
	std::vector<glm::vec3> polytope(simplex.begin(), simplex.end());//the simplex is now a polytope as more points get added to it
	
	std::vector<size_t> faceIndices = { 0, 1, 2,
										0, 3, 1,
										0, 2, 3,
										1, 3, 2 }; // faces stored as indices to the vertices in the polytope

	auto [normals, minFace] = getFaceNormals(polytope, faceIndices);
	
	glm::vec3 minNormal;
	float minDistance = FLT_MAX;
	size_t iterations = 0;

	while (minDistance == FLT_MAX) //minDistance 
	{
		minNormal = glm::vec3(normals[minFace]);//represents normal whose face is closest to the origin
		minDistance = normals[minFace].w;//distance of the closest plane to the origin

		if (iterations++ > GJK_EPA_MAX_ITERATIONS) 
		{
			break;
		}

		glm::vec3 supportPoint = support(colliderA, colliderB, minNormal);//finding a new support point in the direction of the normal
		float supportPointDistance = glm::dot(supportPoint, minNormal);

		//checking to see if new support point is also the closest point in the polytope, if it is then the current normal and min distance
		// are the collision normal and penetration depth respectively

		//due to possible inaccuracies instead of checking equality the difference is checked against a threshold
		if (glm::abs(supportPointDistance - minDistance) > 0.001f)
		{
			minDistance = FLT_MAX;// if new point is the closest in the polytope then minDistance will keep previous value ending the while loop
			
			//existing edges that are needed to construct the new faces
			std::vector<std::pair<size_t, size_t>> uniqueEdges;//an edge is stored as a pair of 2 indices from the faceIndices list

			//reconstrucing the polytope
			for (size_t i = 0; i < normals.size(); i++) 
			{
				if (sameDirection(normals[i], supportPoint))//only faces that can be seen by the new support point need to be processed
				{
					size_t f = i * 3;//indexing through list using face index
					//getting the unique edges
					addUniqueEdge(uniqueEdges, faceIndices, f    , f + 1);
					addUniqueEdge(uniqueEdges, faceIndices, f + 1, f + 2);
					addUniqueEdge(uniqueEdges, faceIndices, f + 2, f    );

					//processed faces and normals are removed from the list and the unchecked ones at the end of the list take their place
					faceIndices[f + 2] = faceIndices.back(); faceIndices.pop_back();
					faceIndices[f + 1] = faceIndices.back(); faceIndices.pop_back();
					faceIndices[f] = faceIndices.back(); faceIndices.pop_back();

					normals[i] = normals.back(); normals.pop_back();

					i--;//i decremented so the normal that was moved to the current place can also be checked
				}
			}

			if (uniqueEdges.size() == 0)//shape cant be reconstructed closest face possibly found and in the outer hull 
			{
				break;
			}

			std::vector<size_t> newFaceIndices;//list storing the new faces which will be constructed using the unique edges
			for (auto [edgeA, edgeB] : uniqueEdges)
			{
				newFaceIndices.push_back(edgeA);//1st vertex of triangle
				newFaceIndices.push_back(edgeB);//2nd vertex of triangle
				newFaceIndices.push_back(polytope.size());//index new point is equal to the size of the polytope (assuming the new point hasn't been added yet)
			}


			polytope.push_back(supportPoint);

			auto [newNormals, newMinFace] = getFaceNormals(polytope, newFaceIndices);//gets normals of the new faces added to the polytope
			
			float newMinDistance = FLT_MAX;
			for (size_t i = 0; i < normals.size(); i++) //getting new minimum distance from the normals list as some elements might've been removed
			{
				if (normals[i].w < newMinDistance) 
				{
					newMinDistance = normals[i].w;
					minFace = i;
				}
			}

			if (newNormals[newMinFace].w < newMinDistance) //getting the smaller min distance from the old and new normals
			{
				//if the new normal's min face is smaller then the index needs to be changed 
				//but if not then move one
				minFace = newMinFace + normals.size();
			}

			//finally joining the new and old face index and normal lists together
			faceIndices.insert(faceIndices.end(), newFaceIndices.begin(), newFaceIndices.end());
			normals.insert(normals.end(), newNormals.begin(), newNormals.end());
		}
	}

	if (minDistance == FLT_MAX)
	{
		//loop exited but no min distance was calculated
		return {};
	}


	CollisionManifold collisionManifold;
	collisionManifold.normal = minNormal;
	collisionManifold.depth = minDistance + 0.001f;

	return collisionManifold;
}

CollisionManifold EPA(Simplex simplex, Collider* colliderA, Transform* transformA, Collider* colliderB, Transform* transformB)
{
	std::vector<glm::vec3> polytope(simplex.begin(), simplex.end());//the simplex is now a polytope as more points get added to it

	std::vector<size_t> faceIndices = { 0, 1, 2,
										0, 3, 1,
										0, 2, 3,
										1, 3, 2 }; // faces stored as indices to the vertices in the polytope

	auto [normals, minFace] = getFaceNormals(polytope, faceIndices);

	glm::vec3 minNormal;
	float minDistance = FLT_MAX;
	size_t iterations = 0;

	while (minDistance == FLT_MAX) //minDistance 
	{
		minNormal = glm::vec3(normals[minFace]);//represents normal whose face is closest to the origin
		minDistance = normals[minFace].w;//distance of the closest plane to the origin

		if (iterations++ > GJK_EPA_MAX_ITERATIONS)
		{
			break;
		}

		glm::vec3 supportPoint = support(colliderA, transformA, colliderB, transformB, minNormal);//finding a new support point in the direction of the normal
		float supportPointDistance = glm::dot(supportPoint, minNormal);

		//checking to see if new support point is also the closest point in the polytope, if it is then the current normal and min distance
		// are the collision normal and penetration depth respectively

		//due to possible inaccuracies instead of checking equality the difference is checked against a threshold
		if (glm::abs(supportPointDistance - minDistance) > 0.001f)
		{
			minDistance = FLT_MAX;// if new point is the closest in the polytope then minDistance will keep previous value ending the while loop

			//existing edges that are needed to construct the new faces
			std::vector<std::pair<size_t, size_t>> uniqueEdges;//an edge is stored as a pair of 2 indices from the faceIndices list

			//reconstrucing the polytope
			for (size_t i = 0; i < normals.size(); i++)
			{
				if (sameDirection(normals[i], supportPoint))//only faces that can be seen by the new support point need to be processed
				{
					size_t f = i * 3;//indexing through list using face index
					//getting the unique edges
					addUniqueEdge(uniqueEdges, faceIndices, f, f + 1);
					addUniqueEdge(uniqueEdges, faceIndices, f + 1, f + 2);
					addUniqueEdge(uniqueEdges, faceIndices, f + 2, f);

					//processed faces and normals are removed from the list and the unchecked ones at the end of the list take their place
					faceIndices[f + 2] = faceIndices.back(); faceIndices.pop_back();
					faceIndices[f + 1] = faceIndices.back(); faceIndices.pop_back();
					faceIndices[f] = faceIndices.back(); faceIndices.pop_back();

					normals[i] = normals.back(); normals.pop_back();

					i--;//i decremented so the normal that was moved to the current place can also be checked
				}
			}

			if (uniqueEdges.size() == 0)//shape cant be reconstructed closest face possibly found and in the outer hull 
			{
				break;
			}

			std::vector<size_t> newFaceIndices;//list storing the new faces which will be constructed using the unique edges
			for (auto [edgeA, edgeB] : uniqueEdges)
			{
				newFaceIndices.push_back(edgeA);//1st vertex of triangle
				newFaceIndices.push_back(edgeB);//2nd vertex of triangle
				newFaceIndices.push_back(polytope.size());//index new point is equal to the size of the polytope (assuming the new point hasn't been added yet)
			}


			polytope.push_back(supportPoint);

			auto [newNormals, newMinFace] = getFaceNormals(polytope, newFaceIndices);//gets normals of the new faces added to the polytope

			float newMinDistance = FLT_MAX;
			for (size_t i = 0; i < normals.size(); i++) //getting new minimum distance from the normals list as some elements might've been removed
			{
				if (normals[i].w < newMinDistance)
				{
					newMinDistance = normals[i].w;
					minFace = i;
				}
			}

			if (newNormals[newMinFace].w < newMinDistance) //getting the smaller min distance from the old and new normals
			{
				//if the new normal's min face is smaller then the index needs to be changed 
				//but if not then move one
				minFace = newMinFace + normals.size();
			}

			//finally joining the new and old face index and normal lists together
			faceIndices.insert(faceIndices.end(), newFaceIndices.begin(), newFaceIndices.end());
			normals.insert(normals.end(), newNormals.begin(), newNormals.end());
		}
	}

	if (minDistance == FLT_MAX)
	{
		//loop exited but no min distance was calculated
		return {};
	}


	CollisionManifold collisionManifold;
	collisionManifold.normal = minNormal;
	collisionManifold.depth = minDistance + 0.001f;

	return collisionManifold;
}

std::pair<std::vector<glm::vec4>, size_t> getFaceNormals(std::vector<glm::vec3> polytope, std::vector<size_t> facesIndices) 
{
	std::vector <glm::vec4> normals; // normals are vec3 but 4th/w component of vector stores the distance of the plane from the origin
	size_t minTriangle = 0; //index of triangle with smallest distance
	float minDistance = FLT_MAX; // minimum distance out of all triangles

	for (size_t i = 0; i < facesIndices.size(); i+=3)//increment by 3 to move to next face as each face is defined by 3 vertices 
	{
		
		glm::vec3 a = polytope[facesIndices[i]];
		glm::vec3 b = polytope[facesIndices[i + 1]];
		glm::vec3 c = polytope[facesIndices[i + 2]];

		glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
		
		float distance = glm::dot(normal, a);
		if (distance < 0) 
		{
			normal *= -1;
			distance *= -1;
		}
		normals.emplace_back(normal, distance);

		if (distance < minDistance) 
		{
			minTriangle = i / 3;// i represents an index for vertices while i/3 represents an index for faces
			minDistance = distance;
		}
	}

	return {normals, minTriangle};//vec4 storing the normal and distance of the plane, and index storing the triangle with the smallest distance
}

void addUniqueEdge(std::vector<std::pair<size_t, size_t>>& edges, const std::vector<size_t>& faceIndices, size_t a, size_t b) 
{
    //      3
	//      /\
	//     /  \
	//    /    \
	//  2 ------ 1  <--- edge in the middle is a duplicate edge for bottom triangle it is edge 1-2 and for the top it is 2-1 (thus a reverse edge)
	//    \    /         this edge is found and then removed, every other edge is a unique edge which will be used to reconstruct the polytope
	//     \  /
	//      \/
	//       0

	//looking for a reverse edge
	auto reverse = std::find(edges.begin(), edges.end(), std::make_pair(faceIndices[b], faceIndices[a]));

	//removing reverse edge from edges list if one is found
	if (reverse != edges.end()) 
	{
		edges.erase(reverse);
	}
	//other wise the edge is unique and added to the edges list 
	else 
	{
		edges.emplace_back(faceIndices[a], faceIndices[b]);
	}
}
