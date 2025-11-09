#include "Physics/Collision/BoxCollider.h"

BoxCollider::BoxCollider() 
	: m_position(glm::vec3(0.f,0.f,0.f)), m_orientation(glm::quat(1.f,0.f,0.f,0.f)), m_scale(glm::vec3(1.f))
{

}

BoxCollider::BoxCollider(Transform transform)
	: m_position(transform.getPosition()), m_orientation(transform.getOrientationQ())/*need to make this a get quaternion instead*/, m_scale(transform.getScale())
{

}

glm::vec3 BoxCollider::findFurthestPoint(const glm::vec3& direction)
{
	/*glm::vec3 maxPoint = glm::vec3(1.f);
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

	or
	*/
	//returns vertex in local space

	//rotating search direcition to local space
	glm::vec3 localDirection = glm::transpose(glm::mat3_cast(m_orientation)) * direction;// R^-1 * direction -> R^T * direction, where R is the orientation of the collider
	glm::vec3 localVertex = glm::vec3(localDirection.x > m_scale.x ? m_scale.x : -m_scale.x, localDirection.y > m_scale.y ? m_scale.y : -m_scale.y, localDirection.z > m_scale.z ? m_scale.z : -m_scale.z);
	
	//world space vertex
	glm::vec3 worldVertex = m_position + m_orientation * localVertex;
	return worldVertex;
}

glm::vec3 BoxCollider::getPosition() { return m_position; }
glm::quat BoxCollider::getOrientation() { return m_orientation; }
glm::vec3 BoxCollider::getScale() { return m_scale; }

void BoxCollider::updateCollider(Transform transform) 
{
//updates collider position, orientation, and scale to the given transform's position, orientation and scale respectively
	m_position = transform.getPosition();
	m_orientation = transform.getOrientationQ();
	m_scale = transform.getScale();
}