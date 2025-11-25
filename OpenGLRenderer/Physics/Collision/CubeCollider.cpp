#include "Physics/Collision/CubeCollider.h"

void CubeCollider::updateCollider(glm::vec3 position, glm::vec3 orientation, glm::vec3 scale)
{
	//applying transformations to the cube
	glm::mat4 translateMat = glm::translate(glm::mat4(1.f), m_position);

	glm::mat4 rotationMat = glm::rotate(glm::mat4(1.f), m_orientation.x, glm::vec3(1.f, 0.f, 0.f)) * //this really messy 
		glm::rotate(glm::mat4(1.f), m_orientation.y, glm::vec3(0.f, 1.f, 0.f)) *
		glm::rotate(glm::mat4(1.f), m_orientation.z, glm::vec3(0.f, 0.f, 1.f));

	glm::mat4 scaleMat = glm::scale(glm::mat4(1.f), m_scale);

	for (int i = 0; i < 8; i++)
	{
		m_vertices[i] = glm::vec3((translateMat * rotationMat * scaleMat) * glm::vec4(m_vertices[i], 1.f));
	}
}

void CubeCollider::updateCollider(glm::mat4 transformMatrix) 
{
	m_position = glm::vec3(transformMatrix * glm::vec4(1.f));
	for (int i = 0; i < 8; i++) 
	{
		m_vertices[i] = glm::vec3(transformMatrix * glm::vec4(m_transformedVertices[i], 1.f));
	}
}

void CubeCollider::updateCollider(Transform transform)
{
	updateCollider(transform.getPosition(), transform.getOrientation(), transform.getScale());
}

CubeCollider::CubeCollider(glm::vec3 position, glm::vec3 orientation, glm::vec3 scale) 
	: m_position(position), m_orientation(orientation), m_scale(scale)
{
	updateCollider(position, orientation, scale);
}

CubeCollider::CubeCollider(glm::vec3& position)
{
	CubeCollider(position, glm::vec3(0.f), glm::vec3(1.f));
}

CubeCollider::CubeCollider(Transform transform) 
{
	updateCollider(transform);
}

CubeCollider::CubeCollider() 
{
	CubeCollider(glm::vec3(0.f), glm::vec3(0.f), glm::vec3(1.f));
}

glm::vec3 CubeCollider::findFurthestPoint(const glm::vec3& direction)
{
	glm::vec3 maxPoint = glm::vec3(1.f);
	float maxDistance = -FLT_MAX;

	for (int i = 0; i < 8; i++) 
	{
		float distance = glm::dot(direction, m_vertices[i]);
		if (distance > maxDistance) 
		{
			
			maxDistance = distance;
			maxPoint = m_vertices[i];
		}
	}

	return maxPoint;
}

glm::vec3 CubeCollider::getPosition() 
{
	return m_position;
}

