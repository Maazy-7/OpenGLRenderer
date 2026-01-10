#include "Physics/Components/Transform.h"

Transform::Transform(glm::vec3 position, glm::vec3 orientation, glm::vec3 scale)
	: m_position(position), m_orientation(orientation), m_orientationQuaternion(glm::quat(orientation)), m_scale(scale), m_transformedMat(glm::mat4(1.f))
{

}

Transform::Transform(glm::vec3 position, glm::quat quaternion, glm::vec3 scale)
	: m_position(position), m_orientation(glm::eulerAngles(quaternion)), m_orientationQuaternion(quaternion), m_scale(scale), m_transformedMat(glm::mat4(1.f))
{

}

Transform::Transform(glm::vec3 position)
	: Transform(position, glm::vec3(0.0f), glm::vec3(1.f))
{
	
}

Transform::Transform()
	: Transform(glm::vec3(0.f), glm::vec3(0.0f), glm::vec3(1.f))
{

}


glm::vec3 Transform::getPosition() const { return m_position; }
glm::vec3 Transform::getOrientation() const { return m_orientation; }
glm::quat Transform::getOrientationQuaternion() const { return m_orientationQuaternion; }
glm::vec3 Transform::getScale() const { return m_scale; }

glm::mat4 Transform::getTransformedMat() const 
{ 
	return glm::translate(glm::mat4(1.0f), m_position) * glm::mat4_cast(m_orientationQuaternion) * glm::scale(glm::mat4(1.0f), m_scale);
}

void Transform::updateTransformMatrix() 
{
	m_transformedMat = glm::translate(glm::mat4(1.0f), m_position) * glm::mat4_cast(m_orientationQuaternion) * glm::scale(glm::mat4(1.0f), m_scale);
}

void Transform::updateTransform(glm::mat4 transform, glm::vec3 scale) 
{
	m_transformedMat = transform * glm::scale(glm::mat4(1.f), scale);
}

void Transform::updateTransform() 
{
	m_transformedMat = glm::translate(glm::mat4(1.f), m_position) *
		glm::mat4_cast(m_orientationQuaternion) *
		glm::scale(glm::mat4(1.f), m_scale);
}

void Transform::updateTransform(glm::vec3 position, glm::quat quaternion) 
{
	//PRINTVEC3(quaternion);
	//PRINTVEC3(m_orientationQuaternion);
	
	m_position = position;
	m_orientationQuaternion = quaternion;
	m_orientation = glm::eulerAngles(quaternion);
}

void Transform::updateTransform(glm::vec3 position, glm::quat quaternion, glm::vec3 scale) 
{
	m_position = position;
	m_orientationQuaternion = quaternion;
	m_orientation = glm::eulerAngles(quaternion);
	m_scale = scale;
}

void Transform::translate(glm::vec3 translateVector) 
{
	m_position += translateVector;
	m_updateTransformMat = true;
}
void Transform::rotate(glm::vec3 rotateVector) //rotates using euler angles
{
	//rotate by a certain amount, adds rotation to orientation
	m_orientation += rotateVector;
	m_orientationQuaternion = m_orientationQuaternion * glm::quat(rotateVector);
	m_updateTransformMat = true;
}
void Transform::rotate(glm::quat quaternion) //rotates using a quaternion
{
	//rotate by a certain amount, adds rotation from quaternion to orientation using quaternion multiplication
	m_orientationQuaternion = m_orientationQuaternion * quaternion;
	m_orientation += glm::eulerAngles(quaternion);
	m_updateTransformMat = true;
}
void Transform::scale(glm::vec3 scaleVector) 
{
	m_scale = glm::vec3(m_scale.x * scaleVector.x, m_scale.y * scaleVector.y, m_scale.z * scaleVector.z);
	m_updateTransformMat = true;
}

void Transform::setTranslate(glm::vec3 translateVector)
{
	m_position = translateVector;
	m_updateTransformMat = true;
}
void Transform::setOrientation(glm::vec3 rotateVector)
{
	m_orientation = rotateVector;
	m_orientationQuaternion = glm::quat(rotateVector);
	m_updateTransformMat = true;
}
void Transform::setOrientation(glm::quat quaternion)
{
	m_orientationQuaternion = quaternion;
	m_orientation = glm::eulerAngles(quaternion);
	m_updateTransformMat = true;
}
void Transform::setScale(glm::vec3 scaleVector)
{
	m_scale = scaleVector;
	m_updateTransformMat = true;
}