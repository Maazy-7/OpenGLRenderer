#pragma once

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

	Transform m_transform;
	Rigidbody m_rigidbody;
	CubeCollider m_collider;
	Collider* M_collider;
	Model m_model;
	Camera* m_camera;
public:

	bool isStatic;//boolean for if object can or cant be moved, TEMPORARY
	GameObject();
	GameObject(Transform* transform, CubeCollider* collider, Model* model);
	GameObject(Transform* transform, CubeCollider* collider, Model* model, Camera* camera);

	GameObject(Transform* transform, Collider* collider, Model* model);
	GameObject(Transform* transform, Collider* collider, Model* model, Camera* camera);
	
	GameObject(glm::vec3 position, Model* model, Camera* camera);

	Transform& transform();
	Rigidbody& rigidbody();
	CubeCollider& collider();
	Model& model();

	void attachCamera(Camera* camera);
	void update(float dt);
	void render(Shader& shader);
	void renderDepth(Shader& shader);

	glm::vec3 getPosition();
	glm::vec3 getOrientation();
	glm::vec3 getScale();
	glm::mat4 getTransformedMat();

	void setAngularVelocity(glm::vec3 angularVelocity); //TEMPORARY

	void setPosition(glm::vec3 position);
	void movePosition(glm::vec3 position);
	void setRotation(glm::vec3 angle);
	void rotate(glm::vec3 angle);
	void setScale(glm::vec3 scale);
	void scale(glm::vec3 scale);

};