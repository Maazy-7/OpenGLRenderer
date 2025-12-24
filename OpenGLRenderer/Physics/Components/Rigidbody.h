#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <utility>

#include "Physics/Components/Transform.h"

class Rigidbody 
{
private:
	glm::vec3 m_position;
	glm::vec3 m_previousPosition;// needed for verlet integration
	glm::vec3 m_velocity;
	glm::vec3 m_acceleration;
	glm::vec3 m_force; //net force currently on object
	glm::quat m_quatOrientation;
	glm::vec3 m_angularVelocity;
	
	float m_mass;
	float m_inverseMass;
	glm::mat3 m_bodyInertia;
	glm::mat3 m_worldInvertia;
	glm::mat3 m_invWorldInertia;

	Transform* m_parentTransform;

	float m_dt;//storing delta time for setting and getting 

	void calculateInertia();

public: 
	
	Rigidbody();
	Rigidbody(glm::vec3 m_position, float mass);

	void update(float dt);

	glm::mat4 getFinalMat4();
	glm::vec3 getVelocity();
	glm::vec3 getPosition();
	glm::quat getOrientation();
	float getMass();

	
	void move(glm::vec3 position);//TEMPORARY

	void setVelocity(glm::vec3 velocity);
	void addVelocity(glm::vec3 velocity);
	void setAngularVelocity(glm::vec3 angularVelocity);
	void addForce(glm::vec3 force);
	void applyImpulse(glm::vec3 impulse);
};