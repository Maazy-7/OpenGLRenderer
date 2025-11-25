#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform 
{
private:
	glm::vec3 m_position;
	glm::vec3 m_orientation;
	glm::quat m_orientationTODO;//will replace euler m_orientation
	glm::vec3 m_scale;

	glm::mat4 m_transformedMat;//matrix with all transformations applied from movements which will then be applied to all components of a game object


public:

	bool m_updateTransformMat = true;
	
	Transform(glm::vec3& position, glm::vec3& orientation, glm::vec3& scale);
	Transform(glm::vec3& position);
	Transform();
	

	glm::vec3 getPosition();
	glm::vec3 getOrientation();
	glm::quat getOrientationQ();
	glm::vec3 getScale();
	glm::mat4 getTransformedMat();

	void updateTransform();
	void updateTransform(glm::mat4 transform, glm::vec3 scale);

	void translate(glm::vec3& translateVector);
	void rotate(glm::vec3& rotateVector);//rotates using euler angles
	void scale(glm::vec3& scaleVector);

	void setTranslate(glm::vec3& translateVector);
	void setOrientation(glm::vec3& rotateVector);//sets rotate using euler angles
	void setScale(glm::vec3& scaleVector);
};