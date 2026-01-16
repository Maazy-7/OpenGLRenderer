#pragma once

#include <memory>

#include "Physics/Components/Transform.h"
#include "Physics/Components/Rigidbody.h"
#include "Physics/Collision/Collider.h"
#include "Physics/Collision/BoxCollider.h"
#include "Model.h"
#include "Camera.h"
#include "Shader.h"

class GameObject 
{
private:

	uint32_t m_ID;

	std::unique_ptr<Transform> m_transform;
	std::unique_ptr<Rigidbody> m_rigidBody;
	std::unique_ptr<Collider> m_collider;
	
	Model* m_model;
	Camera* m_camera;

public:

	bool m_isStatic;//boolean for if object can or cant be moved, TEMPORARY

	GameObject(glm::vec3 position, Model* model, Camera* camera);

	GameObject(std::unique_ptr<Transform> transform, std::unique_ptr<Collider> collider, std::unique_ptr<Rigidbody> rigidbody, Model* model, Camera* camera);
	GameObject(std::unique_ptr<Transform> transform, Model* model, Camera* camera);
	
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

	void setPosition(glm::vec3 position);
	void setRotation(glm::vec3 angle);
	void rotate(glm::vec3 angle);
	void setScale(glm::vec3 scale);
	void scale(glm::vec3 scale);
};