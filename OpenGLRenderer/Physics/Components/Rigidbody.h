#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <utility>

#include "util.h"
#include "Physics/Components/Transform.h"

class PhysicsEngine;
class ContactSolverSystem;
class GameObject;

enum class ShapeType 
{
	Sphere,
	Capsule,
	Box
};

class Rigidbody 
{
private:

	uint32_t m_ID;

	glm::vec3 m_position;
	glm::vec3 m_velocity;
	glm::vec3 m_force;

	glm::quat m_orientation;
	glm::vec3 m_angularVelocity;
	glm::vec3 m_torque;

	float m_mass;
	float m_inverseMass;
	glm::mat3 m_inverseBodyInertia;
	glm::mat3 m_invWorldInertia;

	float m_restitution;
	float m_frictionCoefficient;

	bool m_isStatic;
	bool m_isKinematic;

	Transform* m_parentTransform;

	void calculateInertia(ShapeType shapeType, const glm::vec3& dimensions);
	void updateWorldInertia();


	void applyImpulse(const glm::vec3& impulse, const glm::vec3& point);
public: 
	
	Rigidbody();
	Rigidbody(glm::vec3 position, float mass, bool isStatic = false);
	Rigidbody(glm::vec3 position, float mass, float m_restituion, float frictionCoefficient, Transform* transform = nullptr, bool isStatic = false, bool isKinematic = false);

	glm::vec3 getVelocity() const;
	glm::vec3 getPosition() const;
	glm::quat getOrientation() const;
	glm::vec3 getAngularVelocity() const;
	float getMass() const;
	glm::mat3 getInverseInertiaTensor() const;

	void attachParentTransform(Transform* transform);

	void setStatic(bool isStatic);
	void setID(const uint32_t& ID);
	uint32_t getID() const;

	bool isStatic() const;

	
	void move(glm::vec3 position);//TEMPORARY

	void setAngularVelocity(glm::vec3 angularVelocity);
	void addForce(glm::vec3 force);
	void addForce(const glm::vec3& force, const glm::vec3& point);

	friend PhysicsEngine;
	friend ContactSolverSystem;
	friend GameObject;
};