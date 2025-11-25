#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Physics/Components/Transform.h"

#include <iostream>

class CubeCollider
{
private:
	glm::vec3 m_vertices[8] = {
		glm::vec3( 1.f, 1.f, 1.f),
		glm::vec3(-1.f, 1.f, 1.f),
		glm::vec3(-1.f, 1.f,-1.f),
		glm::vec3( 1.f, 1.f,-1.f),

		glm::vec3( 1.f,-1.f, 1.f),
		glm::vec3(-1.f,-1.f, 1.f),
		glm::vec3(-1.f,-1.f,-1.f),
		glm::vec3( 1.f,-1.f,-1.f),
	};

	glm::vec3 m_transformedVertices[8] = {
		glm::vec3( 1.f, 1.f, 1.f),
		glm::vec3(-1.f, 1.f, 1.f),
		glm::vec3(-1.f, 1.f,-1.f),
		glm::vec3( 1.f, 1.f,-1.f),

		glm::vec3( 1.f,-1.f, 1.f),
		glm::vec3(-1.f,-1.f, 1.f),
		glm::vec3(-1.f,-1.f,-1.f),
		glm::vec3( 1.f,-1.f,-1.f),
	};

	glm::vec3 m_position;
	glm::vec3 m_scale;
	glm::vec3 m_orientation;

public:

	CubeCollider(Transform transform);
	CubeCollider(glm::vec3 position, glm::vec3 orientation, glm::vec3 scale);
	CubeCollider(glm::vec3& position);
	CubeCollider();

	void updateCollider(Transform transform);
	void updateCollider(glm::vec3 position, glm::vec3 orientation, glm::vec3 scale);
	void updateCollider(glm::mat4 transformMatrix);

	glm::vec3 findFurthestPoint(const glm::vec3& direction);
	glm::vec3 getPosition();
};

