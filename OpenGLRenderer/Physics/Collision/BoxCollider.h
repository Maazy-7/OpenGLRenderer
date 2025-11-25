#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <array>

#include "Physics/Collision/Collider.h"
#include "Physics/Components/Transform.h"

struct Face
{
	std::array<int, 4> m_indices;
	Face(int v1, int v2, int v3, int v4) { m_indices = {v1,v2,v3,v4}; }
};

class BoxCollider : Collider
{
private:
	const std::array<glm::vec3, 8> m_vertices = {
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
	};

	glm::vec3 m_position;
	glm::quat m_orientation;
	glm::vec3 m_scale;

	
public:

	BoxCollider();
	BoxCollider(Transform transform);
	glm::vec3 findFurthestPoint(const glm::vec3& direction) override;
	glm::vec3 getPosition() override;
	glm::quat getOrientation() override;
	glm::vec3 getScale() override;

	void updateCollider(Transform transform) override;
};

