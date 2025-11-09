#include "Physics/Components/Rigidbody.h"

Rigidbody::Rigidbody(glm::vec3 position, float mass) 
	: m_position(position), m_mass(mass), m_previousPosition(position), m_velocity(glm::vec3(0.f)), m_acceleration(glm::vec3(0.f)),
	m_force(glm::vec3(0.f)), m_quatOrientation(1.0,0.f,0.f,0.f), m_angularVelocity(glm::vec3(0.f)), m_dt(1.f/60.f)
{
	//TODO: calculating inertia, inverse inertia etc,
	m_inverseMass = mass == 0.0 ? 0.0 : 1.f / mass;
}

Rigidbody::Rigidbody() 
{
	Rigidbody(glm::vec3(0.f), 1.f);
}

glm::mat4 Rigidbody::getFinalMat4() { return glm::translate(glm::mat4(1.f), m_position) * glm::mat4_cast(m_quatOrientation); }
glm::vec3 Rigidbody::getVelocity() { return (m_position - m_previousPosition)/m_dt; }
glm::vec3 Rigidbody::getPosition() { return m_position; }
glm::quat Rigidbody::getOrientation() { return m_quatOrientation; }
float Rigidbody::getMass() { return m_mass; }

void Rigidbody::move(glm::vec3 position) { m_position += position; }//TEMPORARY

void Rigidbody::setVelocity(glm::vec3 velocity) { m_previousPosition = m_position - (m_velocity*m_dt); }//if getVelocity() is called its calculation will return this new set velocity
void Rigidbody::addVelocity(glm::vec3 velocity) { m_previousPosition -= velocity * m_dt; }//if getVelocity() is called it will return the current velocity + this added velocity
void Rigidbody::setAngularVelocity(glm::vec3 angularVelocity) { m_angularVelocity = angularVelocity; }
void Rigidbody::addForce(glm::vec3 force) { m_force += force; }

void Rigidbody::update(float dt) 
{
	m_dt = dt;//storing dt since getting and setting velocity in verlet require dt

//TEMPORARY
	

	//semi explicit euler
	m_acceleration = m_force / m_mass;
	m_velocity += m_acceleration * dt;
	m_position += m_velocity * dt;

	//quaternion integration
	glm::quat quat(0.f, m_angularVelocity*dt);
	m_quatOrientation += 0.5f * m_quatOrientation * quat;
	m_quatOrientation = glm::normalize(m_quatOrientation);

	m_force = glm::vec3(0.0f);
}