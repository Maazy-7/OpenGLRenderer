#include "Physics/Components/Rigidbody.h"

Rigidbody::Rigidbody(glm::vec3 position, float mass, float restituion, float frictionCoefficient, Transform* transform, bool isStatic, bool isKinematic)
	: m_position(position), m_mass(mass), m_isStatic(isStatic), m_velocity(glm::vec3(0.f)), m_force(glm::vec3(0.f)),
	m_orientation(1.0,0.f,0.f,0.f), m_angularVelocity(glm::vec3(0.f)), m_torque(glm::vec3(0.f)), m_parentTransform(transform),
	m_restitution(restituion), m_frictionCoefficient(frictionCoefficient), m_isKinematic(isKinematic)
{
	if (m_isStatic) { mass = 0.f; }
	m_inverseMass = mass == 0.0 ? 0.0 : 1.f / mass;
}

Rigidbody::Rigidbody(glm::vec3 position, float mass, bool isStatic) 
	:Rigidbody(position, mass, 0.5f, 0.45f, nullptr, isStatic)
{

}

Rigidbody::Rigidbody() 
	: Rigidbody(glm::vec3(0.f), 1.f)
{

}

glm::vec3 Rigidbody::getVelocity() const { return m_velocity; }
glm::vec3 Rigidbody::getPosition() const { return m_position; }
glm::quat Rigidbody::getOrientation() const { return m_orientation; }
glm::vec3 Rigidbody::getAngularVelocity() const { return m_angularVelocity; }
float Rigidbody::getMass() const { return m_mass; }
glm::mat3 Rigidbody::getInverseInertiaTensor() const { return m_invWorldInertia; }

void Rigidbody::attachParentTransform(Transform* transform) 
{
	m_parentTransform = transform;
}

void Rigidbody::setStatic(bool isStatic)
{
	m_isStatic = isStatic;
	m_mass = 0.0f;
	m_inverseMass = 0.f;
}

void Rigidbody::setID(const uint32_t& ID)
{
	m_ID = ID;
}

uint32_t Rigidbody::getID() const 
{
	return m_ID;
}

bool Rigidbody::isStatic() const 
{
	return m_isStatic;
}

void Rigidbody::calculateInertia(ShapeType shapeType, const glm::vec3& dimensions) //dimensions could be w x h x l or radius and height or just radius 
{
	constexpr float oneOverTwelve = 1.f / 12.f;

	//dimensions for Box w x h x l -> vec3(w, h, l)
	//dimensions for Capsule radius and height -> vec3(radius, height, radius)
	//dimensions for Sphere radius -> vec3(radius, radius, radius)

	glm::mat3 inertia;
	float Ix, Iy, Iz, I;
	switch (shapeType)
	{
	case ShapeType::Sphere:
		I = (2.f / 5.f) * m_mass * dimensions.x * dimensions.x;
		inertia[0][0] = I;
		inertia[1][1] = I;
		inertia[2][2] = I;
		break;

	case ShapeType::Capsule:
		Iy = oneOverTwelve * m_mass * (3 * dimensions.x * dimensions.x + dimensions.y * dimensions.y);
		Ix = Iy + 0.25f * m_mass * dimensions.x * dimensions.x;
		inertia[0][0] = Ix;
		inertia[1][1] = Iy;
		inertia[2][2] = Ix;
		break;

	case ShapeType::Box:
		Ix = oneOverTwelve * m_mass * (dimensions.y * dimensions.y + dimensions.z * dimensions.z);
		Iy = oneOverTwelve * m_mass * (dimensions.x * dimensions.x + dimensions.z * dimensions.z);
		Iz = oneOverTwelve * m_mass * (dimensions.y * dimensions.y + dimensions.y * dimensions.y);
		inertia[0][0] = Ix;
		inertia[1][1] = Iy;
		inertia[2][2] = Iz;
		break;

	default:
		LOG("Unrecognized shape type");
		break;
	}

	m_inverseBodyInertia = glm::mat3(0.f);

	if (m_mass > 0.f) //since inertia is a diagonal matrix, reciprocal of diagonal entries will be used to calculate body inertia
	{
		m_inverseBodyInertia[0][0] = inertia[0][0] > 0.f ? 1.f / inertia[0][0] : 0.f;
		m_inverseBodyInertia[1][1] = inertia[1][1] > 0.f ? 1.f / inertia[1][1] : 0.f;
		m_inverseBodyInertia[2][2] = inertia[2][2] > 0.f ? 1.f / inertia[2][2] : 0.f;
	}

	updateWorldInertia();
}

void Rigidbody::move(glm::vec3 position) { m_position += position; }//TEMPORARY

void Rigidbody::setAngularVelocity(glm::vec3 angularVelocity) { m_angularVelocity = angularVelocity; }

void Rigidbody::addForce(glm::vec3 force) { m_force += force; }
void Rigidbody::addForce(const glm::vec3& force, const glm::vec3& point) { m_force += force; m_torque += glm::cross(point, force); }

void Rigidbody::applyImpulse(const glm::vec3& impulse, const glm::vec3& point) 
{
	if (m_inverseMass == 0.f) { return; }
	m_velocity += impulse * m_inverseMass;
	m_angularVelocity += m_invWorldInertia * glm::cross(point, impulse);
}

void Rigidbody::updateWorldInertia() 
{
	if (m_mass == 0.f) { return; }
	glm::mat3 R = glm::mat3_cast(m_orientation);
	m_invWorldInertia = R * m_inverseBodyInertia * glm::transpose(R);
}