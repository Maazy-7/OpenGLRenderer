#include "Physics/Collision/BoxCollider.h"

BoxCollider::BoxCollider()
	: m_halfExtents(glm::vec3(1.f))
{
	m_type = ColliderType::Box;
}

BoxCollider::BoxCollider(glm::vec3 halfExtents)
	: m_halfExtents(halfExtents)
{
	m_type = ColliderType::Box;
}

BoxCollider::BoxCollider(glm::vec3 halfExtents, Transform* transform)
    : m_halfExtents(halfExtents)
{
    m_parentTransform = transform;
    m_type = ColliderType::Box;
}

ColliderType BoxCollider::getType() const
{
    return m_type;
}

void BoxCollider::attachParentTransform(Transform* transform) 
{
    m_parentTransform = transform;
}

AABB BoxCollider::getAABB() const
{
    return m_colliderAABB;
}

glm::vec3 BoxCollider::findFurthestPoint(const glm::vec3& direction) const
{
	//rotating search direcition to local space
	glm::vec3 localDirection = glm::conjugate(m_parentTransform->getOrientationQuaternion()) * direction;// Q^-1 * direction where Q is the orientation of the collider/transform
	glm::vec3 localVertex = glm::vec3(localDirection.x > 0 ? m_halfExtents.x : -m_halfExtents.x, localDirection.y > 0 ? m_halfExtents.y : -m_halfExtents.y, localDirection.z > 0 ? m_halfExtents.z : -m_halfExtents.z);
	
	//world space vertex
	glm::vec3 worldVertex = m_parentTransform->getPosition() + m_parentTransform->getOrientationQuaternion() * localVertex;
	return worldVertex;
}

Face BoxCollider::getBestFace(const glm::vec3& normal) 
{
	glm::vec3 localNormal = glm::conjugate(m_parentTransform->getOrientationQuaternion()) * normal;
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
    bestFace.m_normal = m_parentTransform->getOrientationQuaternion() * bestFace.m_normal;
    for (int i = 0; i < 4; i++) 
    {
        bestFace.m_vertices[i] = m_parentTransform->getPosition() + m_parentTransform->getOrientationQuaternion() * bestFace.m_vertices[i];
    }

    return bestFace;
}

glm::vec3 BoxCollider::getHalfExtents() const 
{
    return m_halfExtents;
}
glm::vec3 BoxCollider::getPosition() const
{
    return m_parentTransform->getPosition();
}
glm::quat BoxCollider::getOrientation() const
{
    return m_parentTransform->getOrientationQuaternion();
}