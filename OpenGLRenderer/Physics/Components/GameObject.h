#pragma once

#include <memory>

#include "Physics/Components/Transform.h"
#include "Physics/Components/Rigidbody.h"
#include "Physics/Collision/Collider.h"
#include "Physics/Collision/BoxCollider.h"
#include "Physics/Collision/CubeCollider.h"
#include "Model.h"
#include "Camera.h"
#include "Shader.h"

class GameObject 
{
private:

	Transform m_transform;//temp
	Rigidbody m_rigidbody;//temp
	CubeCollider m_collider;//temp

	uint32_t m_ID;

	std::unique_ptr<Transform> M_transform;
	std::unique_ptr<Rigidbody> M_rigidBody;
	std::unique_ptr<Collider> M_collider;
	
	Model* m_model;
	Camera* m_camera;

public:

	bool m_isStatic;//boolean for if object can or cant be moved, TEMPORARY
	GameObject();
	GameObject(Transform* transform, CubeCollider* collider, Model* model);
	GameObject(Transform* transform, CubeCollider* collider, Model* model, Camera* camera);

	GameObject(Transform* transform, Collider* collider, Model* model);
	GameObject(Transform* transform, Collider* collider, Model* model, Camera* camera);
	
	GameObject(glm::vec3 position, Model* model, Camera* camera);

	GameObject(std::unique_ptr<Transform> transform, std::unique_ptr<Collider> collider, std::unique_ptr<Rigidbody> rigidbody, Model* model, Camera* camera);
	GameObject(std::unique_ptr<Transform> transform, Model* model, Camera* camera);
	//GameObject(glm::vec3 position, Model* model, Camera* camera);

	
	CubeCollider& cCollider();
	
	Transform* getTransform();
	Rigidbody* getRigidbody();
	Collider* getCollider();
	Model* model();

	void attachCamera(Camera* camera);
	void update(float dt);
	void render(Shader& shader);
	void renderDepth(Shader& shader);

	glm::vec3 getPosition() const;
	glm::vec3 getOrientation() const;
	glm::vec3 getScale() const;
	glm::mat4 getTransformedMat() const;

	void setStatic(bool isStatic);
	void setID(const uint32_t& ID);

	void setAngularVelocity(glm::vec3 angularVelocity); //TEMPORARY

	void setPosition(glm::vec3 position);
	void movePosition(glm::vec3 position);
	void setRotation(glm::vec3 angle);
	void rotate(glm::vec3 angle);
	void setScale(glm::vec3 scale);
	void scale(glm::vec3 scale);

};