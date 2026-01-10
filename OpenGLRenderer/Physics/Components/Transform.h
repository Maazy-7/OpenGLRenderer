#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include "util.h"

class Transform 
{
private:
	glm::vec3 m_position;
	glm::vec3 m_orientation;
	glm::quat m_orientationQuaternion;//will replace euler m_orientation
	glm::vec3 m_scale;

	glm::mat4 m_transformedMat;//matrix with all transformations applied from movements which will then be applied to all components of a game object
	bool m_updateTransformMat = true;

	void updateTransformMatrix();//changes transform martix but does NOT change position, orientation or scale

public:	
	Transform(glm::vec3 position, glm::vec3 orientation, glm::vec3 scale);
	Transform(glm::vec3 position, glm::quat quaternion, glm::vec3 scale);
	Transform(glm::vec3 position);
	Transform();
	

	glm::vec3 getPosition() const;
	glm::vec3 getOrientation() const;
	glm::quat getOrientationQuaternion() const;
	glm::vec3 getScale() const;
	glm::mat4 getTransformedMat() const;

	void updateTransform();
	void updateTransform(glm::mat4 transform, glm::vec3 scale);
	void updateTransform(glm::vec3 position, glm::quat quaternion);
	void updateTransform(glm::vec3 position, glm::quat quaternion, glm::vec3 scale);

	void translate(glm::vec3 translateVector);
	void rotate(glm::vec3 rotateVector);//rotates using euler angles
	void rotate(glm::quat quaternion);//rotates using a quaternion
	void scale(glm::vec3 scaleVector);

	void setTranslate(glm::vec3 translateVector);
	void setOrientation(glm::vec3 rotateVector);//sets rotate using euler angles
	void setOrientation(glm::quat quaternion);//sets rotate using a quaternion
	void setScale(glm::vec3 scaleVector);
};