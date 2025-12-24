#include "Physics/Collision/BoxCollider.h"

BoxCollider::BoxCollider()
	: m_halfExtents(glm::vec3(1.f))
{
	m_type = Box;
}

BoxCollider::BoxCollider(glm::vec3 halfExtents)
	: m_halfExtents(halfExtents)
{
	m_type = Box;
}

glm::vec3 BoxCollider::findFurthestPoint(const glm::vec3& direction, const Transform& transform)
{
	//rotating search direcition to local space
	glm::vec3 localDirection = glm::conjugate(transform.getOrientationQuaternion()) * direction;// Q^-1 * direction where Q is the orientation of the collider/transform
	glm::vec3 localVertex = glm::vec3(localDirection.x > 0 ? m_halfExtents.x : -m_halfExtents.x, localDirection.y > 0 ? m_halfExtents.y : -m_halfExtents.y, localDirection.z > 0 ? m_halfExtents.z : -m_halfExtents.z);
	
	//world space vertex
	glm::vec3 worldVertex = transform.getPosition() + transform.getOrientationQuaternion() * localVertex;
	return worldVertex;
}

Face BoxCollider::getBestFace(const glm::vec3& normal, const Transform& transform) 
{
	glm::vec3 localNormal = glm::conjugate(transform.getOrientationQuaternion()) * normal;
	glm::vec3 absNormal = glm::abs(localNormal);
	Face bestFace;

    if (absNormal.x > absNormal.y && absNormal.x > absNormal.z) {
        // X is dominant: Best face is either +X or -X
        float sign = (localNormal.x > 0) ? 1.0f : -1.0f;
        bestFace.m_normal = glm::vec3(sign, 0, 0);
        bestFace.m_vertices[0] = glm::vec3(sign * m_halfExtents.x,  m_halfExtents.y,  m_halfExtents.z);
        bestFace.m_vertices[1] = glm::vec3(sign * m_halfExtents.x, -m_halfExtents.y,  m_halfExtents.z);
        bestFace.m_vertices[2] = glm::vec3(sign * m_halfExtents.x, -m_halfExtents.y, -m_halfExtents.z);
        bestFace.m_vertices[3] = glm::vec3(sign * m_halfExtents.x,  m_halfExtents.y, -m_halfExtents.z);
    }
    else if (absNormal.y > absNormal.z) {
        // Y is dominant
        float sign = (localNormal.y > 0) ? 1.0f : -1.0f;
        bestFace.m_normal = glm::vec3(0, sign, 0);
        bestFace.m_vertices[0] = glm::vec3( m_halfExtents.x, sign * m_halfExtents.y,  m_halfExtents.z);
        bestFace.m_vertices[1] = glm::vec3(-m_halfExtents.x, sign * m_halfExtents.y,  m_halfExtents.z);
        bestFace.m_vertices[2] = glm::vec3(-m_halfExtents.x, sign * m_halfExtents.y, -m_halfExtents.z);
        bestFace.m_vertices[3] = glm::vec3( m_halfExtents.x, sign * m_halfExtents.y, -m_halfExtents.z);
    }
    else {
        // Z is dominant
        float sign = (localNormal.z > 0) ? 1.0f : -1.0f;
        bestFace.m_normal = glm::vec3(0, 0, sign);
        bestFace.m_vertices[0] = glm::vec3( m_halfExtents.x,  m_halfExtents.y, sign * m_halfExtents.z);
        bestFace.m_vertices[1] = glm::vec3(-m_halfExtents.x,  m_halfExtents.y, sign * m_halfExtents.z);
        bestFace.m_vertices[2] = glm::vec3(-m_halfExtents.x, -m_halfExtents.y, sign * m_halfExtents.z);
        bestFace.m_vertices[3] = glm::vec3( m_halfExtents.x, -m_halfExtents.y, sign * m_halfExtents.z);
    }

    //transforming face normal and vertices back to world space
    bestFace.m_normal = transform.getOrientationQuaternion() * bestFace.m_normal;
    for (int i = 0; i < 4; i++) 
    {
        bestFace.m_vertices[i] = transform.getPosition() + transform.getOrientationQuaternion() * bestFace.m_vertices[i];
    }

    return bestFace;
}