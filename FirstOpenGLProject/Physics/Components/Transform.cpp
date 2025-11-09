#include "Physics/Components/Transform.h"

Transform::Transform(glm::vec3& position, glm::vec3& orientation, glm::vec3& scale)
	: m_position(position), m_orientation(orientation), m_scale(scale), m_transformedMat(glm::mat4(1.f))
{

}

Transform::Transform() 
	: m_position(glm::vec3(0.f)), m_orientation(glm::vec3(0.f)), m_scale(glm::vec3(1.f)), m_transformedMat(glm::mat4(1.f))
{

}

Transform::Transform(glm::vec3& position) 
	: m_position(position), m_orientation(glm::vec3(0.f)), m_scale(glm::vec3(1.f)), m_transformedMat(glm::mat4(1.f))
{
	
}

glm::vec3 Transform::getPosition() { return m_position; }
glm::vec3 Transform::getOrientation() { return m_orientation; }
glm::quat Transform::getOrientationQ() { return m_orientationTODO; }
glm::vec3 Transform::getScale() { return m_scale; }
glm::mat4 Transform::getTransformedMat() { return m_transformedMat; }

void Transform::updateTransform(glm::mat4 transform, glm::vec3 scale) 
{
	m_transformedMat = transform * glm::scale(glm::mat4(1.f), scale);
}

void Transform::updateTransform() 
{
	m_transformedMat = glm::translate(glm::mat4(1.f), m_position) *
		glm::rotate(glm::mat4(1.f), m_orientation.x, glm::vec3(1.f, 0.f, 0.f)) *
		glm::rotate(glm::mat4(1.f), m_orientation.y, glm::vec3(0.f, 1.f, 0.f)) *
		glm::rotate(glm::mat4(1.f), m_orientation.z, glm::vec3(0.f, 0.f, 1.f)) *
		glm::scale(glm::mat4(1.f), m_scale);
}

void Transform::translate(glm::vec3& translateVector) 
{
	m_position += translateVector;
	m_updateTransformMat = true;
}
void Transform::rotate(glm::vec3& rotateVector) //rotates using euler angles
{
	//rotate by a certain amount, adds rotation to orientation
	m_orientation += rotateVector;
	m_updateTransformMat = true;
}
void Transform::scale(glm::vec3& scaleVector) 
{
	m_scale = glm::vec3(m_scale.x * scaleVector.x, m_scale.y * scaleVector.y, m_scale.z * scaleVector.z);
	m_updateTransformMat = true;
}

void Transform::setTranslate(glm::vec3& translateVector)
{
	m_position = translateVector;
	m_updateTransformMat = true;
}
void Transform::setOrientation(glm::vec3& rotateVector)
{
	m_orientation = rotateVector;
	m_updateTransformMat = true;
}
void Transform::setScale(glm::vec3& scaleVector)
{
	m_scale = scaleVector;
	m_updateTransformMat = true;
}