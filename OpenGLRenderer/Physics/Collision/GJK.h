#pragma once

#include <glm/glm.hpp>
#include <initializer_list>
#include <iterator>
#include <array>
#include <iostream>
#include <vector>
#include <utility>

#include "Physics/Collision/CubeCollider.h"
#include "Physics/Collision/Collider.h"
#include "Physics/Collision/CollisionManifold.h"
#include "Physics/Components/Transform.h"

struct Simplex 
{
private:
	std::array<glm::vec3, 4> m_points;
	size_t m_size;

public:
	Simplex();

	Simplex& operator=(std::initializer_list<glm::vec3> list);
	glm::vec3& operator[](unsigned i);
	
	auto begin();
	auto end();

	void pushToFront(glm::vec3 point);
	size_t size();
};

//helper functions for gjk
glm::vec3 support(CubeCollider* colliderA, CubeCollider* colliderB, const glm::vec3& direction);
glm::vec3 support(Collider* colliderA, Collider* colliderB, const glm::vec3& direction);
bool sameDirection(const glm::vec3& a, const glm::vec3& b);

//helper functions for EPA
std::pair<std::vector<glm::vec4>, size_t> getFaceNormals(std::vector<glm::vec3> polytope, std::vector<size_t> facesIndices);
void addUniqueEdge(std::vector<std::pair<size_t, size_t>>& edges, const std::vector<size_t>& faceIndices, size_t a, size_t b);

#define GJK_EPA_MAX_ITERATIONS 32


std::pair<Simplex, bool> GJK(CubeCollider* colliderA, CubeCollider* colliderB);
std::pair<Simplex, bool> GJK(Collider* colliderA, Collider* colliderB);
CollisionManifold EPA(Simplex simplex, CubeCollider* colliderA, CubeCollider* colliderB);
CollisionManifold EPA(Simplex simplex, Collider* colliderA, Collider* colliderB);

//handles cases for the simplex
bool nextSimplex(Simplex& simplex, glm::vec3& direction);
//possible simplex cases to handle
bool lineCase(Simplex& simplex, glm::vec3& direction);
bool triangleCase(Simplex& simplex, glm::vec3& direction);
bool tetrahedronCase(Simplex& simplex, glm::vec3& direction);