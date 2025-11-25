#include "GameObject.h"


GameObject::GameObject(glm::vec3 position, Model* model, Camera* camera)
	: m_transform(Transform(position)), m_rigidbody(Rigidbody(position, 1.0f)), m_collider(CubeCollider(position)), m_model(*model), m_camera(camera)
{

}

GameObject::GameObject(Transform* transform, CubeCollider* collider, Model* model, Camera* camera) 
	: m_transform(*transform), m_collider(*collider), m_model(*model), m_camera(camera)
{

}

GameObject::GameObject(Transform* transform, CubeCollider* collider, Model* model)
	: m_transform(*transform), m_collider(*collider), m_model(*model), m_camera(nullptr)
{

}

GameObject::GameObject(Transform* transform, Collider* collider, Model* model, Camera* camera)
	: m_transform(*transform), M_collider(collider), m_model(*model), m_camera(camera)
{

}

GameObject::GameObject(Transform* transform, Collider* collider, Model* model)
	: m_transform(*transform), M_collider(collider), m_model(*model), m_camera(nullptr)
{

}

void GameObject::update(float dt) //currently really messy but will fix
{
	m_rigidbody.update(dt);
	m_transform.updateTransform();
	m_collider.updateCollider(m_transform.getTransformedMat());
	//m_transform.m_updateTransformMat = false;
	
}

void GameObject::render(Shader& shader) 
{
	shader.use();
	shader.setMatrix4f("model", m_transform.getTransformedMat());
	//shader.setMatrix4f("view", m_camera->getView());
	//shader.setMatrix4f("projection", m_camera->getProjection());
	m_model.draw(shader);
}

void GameObject::renderDepth(Shader& shader) 
{
	shader.use();
	shader.setMatrix4f("model", m_transform.getTransformedMat());
	m_model.draw(shader);
}

void GameObject::attachCamera(Camera* camera)
{
	m_camera = camera;
}

Transform& GameObject::transform() { return m_transform; }
Rigidbody& GameObject::rigidbody() { return m_rigidbody; }
CubeCollider& GameObject::collider() { return m_collider; }
Model& GameObject::model() { return m_model; }

glm::vec3 GameObject::getPosition() { return m_transform.getPosition(); }
glm::vec3 GameObject::getOrientation() { return m_transform.getOrientation(); } //returns euler angles
glm::vec3 GameObject::getScale() { return m_transform.getScale(); }
glm::mat4 GameObject::getTransformedMat() { return m_transform.getTransformedMat(); }

void GameObject::setPosition(glm::vec3 position) { m_transform.setTranslate(position); }
void GameObject::movePosition(glm::vec3 position) { m_transform.translate(position); m_rigidbody.move(position); }
void GameObject::setRotation(glm::vec3 angle) { m_transform.setOrientation(angle); }
void GameObject::rotate(glm::vec3 angle) { m_transform.rotate(angle); }
void GameObject::setScale(glm::vec3 scale) { m_transform.setScale(scale); }
void GameObject::scale(glm::vec3 scale) { m_transform.scale(scale); }

void GameObject::setAngularVelocity(glm::vec3 angularVelocity) { m_rigidbody.setAngularVelocity(angularVelocity); }