#include "GameObject.h"

GameObject::GameObject(std::unique_ptr<Transform> transform, std::unique_ptr<Collider> collider, std::unique_ptr<Rigidbody> rigidbody, Model* model, Camera* camera)
	: m_transform(std::move(transform)), 
	m_collider(std::move(collider)), 
	m_rigidBody(std::move(rigidbody)), 
	m_model(model), 
	m_camera(camera)
{
	m_collider->attachParentRigidBody(m_rigidBody.get());
	m_collider->updateAABB();
	m_rigidBody->attachParentTransform(m_transform.get());
	m_rigidBody->calculateInertia(static_cast<ShapeType>((int)m_collider->getType()), m_collider->getDimensionsAsVec3());
}

GameObject::GameObject(std::unique_ptr<Transform> transform, Model* model, Camera* camera)
	: m_transform(std::move(transform)), 
	m_collider(std::make_unique<BoxCollider>(glm::vec3(1.f), m_transform.get())),
	m_rigidBody(std::make_unique<Rigidbody>(m_transform->getPosition(), 1.f)),
	m_model(model),
	m_camera(camera)
{
	//default gameobject with box collider of size 1 (halfExtents of 0.5) rigid body with mass of 1
	m_collider->attachParentRigidBody(m_rigidBody.get());
	m_collider->updateAABB();
	m_rigidBody->attachParentTransform(m_transform.get());
	m_rigidBody->calculateInertia(static_cast<ShapeType>((int)m_collider->getType()), m_collider->getDimensionsAsVec3());
}

GameObject::GameObject(glm::vec3 position, Model* model, Camera* camera)
	: m_transform(std::make_unique<Transform>(position)),
	m_collider(std::make_unique<BoxCollider>(glm::vec3(1.f))),
	m_rigidBody(std::make_unique<Rigidbody>(m_transform->getPosition(), 1.f)),
	m_model(model),
	m_camera(camera)
{
	//default gameobject with box collider of size 1 (halfExtents of 0.5) rigid body with mass of 1
}

void GameObject::update(float dt) //currently really messy but will fix
{
	//m_transform.updateTransform();
	//m_collider.updateCollider(m_transform->getTransformedMat());
	//m_transform.m_updateTransformMat = false;
}

void GameObject::render(Shader& shader) 
{
	shader.use();
	shader.setMatrix4f("model", m_transform->getTransformedMat());
	//shader.setMatrix4f("view", m_camera->getView());
	//shader.setMatrix4f("projection", m_camera->getProjection());
	m_model->draw(shader);
}

void GameObject::renderDepth(Shader& shader) 
{
	shader.use();
	shader.setMatrix4f("model", m_transform->getTransformedMat());
	m_model->draw(shader);
}

void GameObject::setStatic(bool isStatic) 
{
	m_isStatic = isStatic;
	m_rigidBody->setStatic(isStatic);
}

void GameObject::setID(const uint32_t& ID) 
{
	m_ID = ID;
	if (m_rigidBody) 
	{
		m_rigidBody->setID(ID);
	}
}

void GameObject::attachCamera(Camera* camera)
{
	m_camera = camera;
}

Transform* GameObject::getTransform() { return m_transform.get(); }
Rigidbody* GameObject::getRigidbody() { return m_rigidBody.get(); }
Collider* GameObject::getCollider() { return m_collider.get(); }
Model* GameObject::model() { return m_model; }

glm::vec3 GameObject::getPosition() const { return m_transform->getPosition(); }
glm::vec3 GameObject::getOrientation() const { return m_transform->getOrientation(); } //returns euler angles
glm::vec3 GameObject::getScale() const { return m_transform->getScale(); }
glm::mat4 GameObject::getTransformedMat() const { return m_transform->getTransformedMat(); }

void GameObject::setPosition(glm::vec3 position) { m_transform->setTranslate(position); }
void GameObject::setRotation(glm::vec3 angle) { m_transform->setOrientation(angle); }
void GameObject::rotate(glm::vec3 angle) { m_transform->rotate(angle); }
void GameObject::setScale(glm::vec3 scale) { m_transform->setScale(scale); m_collider->setColliderScale(scale); }
void GameObject::scale(glm::vec3 scale) { m_transform->scale(scale); }