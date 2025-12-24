#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <array>

#include "Physics/Collision/Collider.h"
#include "Physics/Components/Transform.h"

struct Face
{
	std::array<glm::vec3, 4> m_vertices;
	glm::vec3 m_normal;
};

class BoxCollider : public Collider
{
private:
	/*const std::array<glm::vec3, 8> m_vertices = {
	glm::vec3( 1.f, 1.f, 1.f),
	glm::vec3( 1.f,-1.f, 1.f),
	glm::vec3(-1.f,-1.f, 1.f),
	glm::vec3(-1.f, 1.f, 1.f),
	
	glm::vec3( 1.f, 1.f,-1.f),
	glm::vec3( 1.f,-1.f,-1.f),
	glm::vec3(-1.f,-1.f,-1.f),
	glm::vec3(-1.f, 1.f,-1.f), };

	const std::array<Face, 6> m_faces = {//Faces used to index vertex list, a Face represents a group of vertices
     Face(0,1,2,3),//front face
	 Face(3,2,6,7),//right face
	 Face(7,6,5,4),//back face
	 Face(4,5,1,0),//left face
	 Face(1,2,6,5),//bottom face
	 Face(0,3,7,4),//top face
	};

	const std::array<glm::vec3, 6> m_normals {//Normals used to index face list, A normal represents the direction of a face
	glm::vec3( 0.f, 0.f, 1.f),//front
	glm::vec3( 1.f, 0.f, 0.f),//right
	glm::vec3( 0.f, 0.f,-1.f),//back
	glm::vec3(-1.f, 0.f, 0.f),//left
	glm::vec3( 0.f,-1.f ,0.f),//bottom
	glm::vec3( 0.f, 1.f, 0.f)//top
	};*/

	glm::vec3 m_halfExtents;

public:

	BoxCollider();
	BoxCollider(glm::vec3 halfExtents);
	glm::vec3 findFurthestPoint(const glm::vec3& direction, const Transform& transform) override;
	Face getBestFace(const glm::vec3& normal, const Transform& transform);
};

