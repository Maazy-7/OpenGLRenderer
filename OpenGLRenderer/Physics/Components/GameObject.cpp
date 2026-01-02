#include "GameObject.h"


GameObject::GameObject(glm::vec3 position, Model* model, Camera* camera)
	: m_transform(Transform(position)), m_rigidbody(Rigidbody(position, 1.0f)), m_collider(CubeCollider(position)), m_model(model), m_camera(camera)
{

}

GameObject::GameObject(Transform* transform, CubeCollider* collider, Model* model, Camera* camera) 
	: m_transform(*transform), m_collider(*collider), m_model(model), m_camera(camera)
{

}

GameObject::GameObject(Transform* transform, CubeCollider* collider, Model* model)
	: m_transform(*transform), m_collider(*collider), m_model(model), m_camera(nullptr)
{

}

GameObject::GameObject(Transform* transform, Collider* collider, Model* model, Camera* camera)
	: m_transform(*transform), M_collider(collider), m_model(model), m_camera(camera)
{

}

GameObject::GameObject(Transform* transform, Collider* collider, Model* model)
	: m_transform(*transform), M_collider(collider), m_model(model), m_camera(nullptr)
{

}

GameObject::GameObject(std::unique_ptr<Transform> transform, std::unique_ptr<Collider> collider, std::unique_ptr<Rigidbody> rigidbody, Model* model, Camera* camera)
	: M_transform(std::move(transform)), 
	M_collider(std::move(collider)), 
	M_rigidBody(std::move(rigidbody)), 
	m_model(model), 
	m_camera(camera)
{

}

GameObject::GameObject(std::unique_ptr<Transform> transform, Model* model, Camera* camera)
	: M_transform(std::move(transform)), 
	M_collider(std::make_unique<BoxCollider>(glm::vec3(1.f), M_transform.get())),
	M_rigidBody(std::make_unique<Rigidbody>(M_transform->getPosition(), 1.f)),
	m_model(model),
	m_camera(camera)
{
	//default gameobject with box collider of size 1 (halfExtents of 0.5) rigid body with mass of 1
}

/*GameObject::GameObject(glm::vec3 position, Model* model, Camera* camera)
	: M_transform(std::make_unique<Transform>(position)),
	M_collider(std::make_unique<BoxCollider>(glm::vec3(1.f))),
	M_rigidBody(std::make_unique<Rigidbody>(M_transform->getPosition(), 1.f)),
	m_model(model),
	m_camera(camera)
{
	//default gameobject with box collider of size 1 (halfExtents of 0.5) rigid body with mass of 1
}*/

void GameObject::update(float dt) //currently really messy but will fix
{
	m_rigidbody.update(dt);
	m_transform.updateTransform();
	m_collider.updateCollider(M_transform->getTransformedMat());
	//m_transform.m_updateTransformMat = false;
}

void GameObject::render(Shader& shader) 
{
	shader.use();
	shader.setMatrix4f("model", M_transform->getTransformedMat());
	//shader.setMatrix4f("view", m_camera->getView());
	//shader.setMatrix4f("projection", m_camera->getProjection());
	m_model->draw(shader);
}

void GameObject::renderDepth(Shader& shader) 
{
	shader.use();
	shader.setMatrix4f("model", M_transform->getTransformedMat());
	m_model->draw(shader);
}

void GameObject::attachCamera(Camera* camera)
{
	m_camera = camera;
}

CubeCollider& GameObject::cCollider() { return m_collider; }

Transform* GameObject::getTransform() { return M_transform.get(); }
Rigidbody* GameObject::getRigidbody() { return M_rigidBody.get(); }
Collider* GameObject::getCollider() { return M_collider.get(); }
Model* GameObject::model() { return m_model; }

glm::vec3 GameObject::getPosition() { return M_transform->getPosition(); }
glm::vec3 GameObject::getOrientation() { return M_transform->getOrientation(); } //returns euler angles
glm::vec3 GameObject::getScale() { return M_transform->getScale(); }
glm::mat4 GameObject::getTransformedMat() { return M_transform->getTransformedMat(); }

void GameObject::setPosition(glm::vec3 position) { M_transform->setTranslate(position); }
void GameObject::movePosition(glm::vec3 position) { M_transform->translate(position); m_rigidbody.move(position); }
void GameObject::setRotation(glm::vec3 angle) { M_transform->setOrientation(angle); }
void GameObject::rotate(glm::vec3 angle) { M_transform->rotate(angle); }
void GameObject::setScale(glm::vec3 scale) { M_transform->setScale(scale); }
void GameObject::scale(glm::vec3 scale) { M_transform->scale(scale); }

void GameObject::setAngularVelocity(glm::vec3 angularVelocity) { m_rigidbody.setAngularVelocity(angularVelocity); }