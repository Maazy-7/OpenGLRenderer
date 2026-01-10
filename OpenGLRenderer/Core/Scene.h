#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "Physics/Core/PhysicsEngine.h"
#include "Physics/Components/GameObject.h"
#include "Rendering/Light.h"
#include "Rendering/ShadowCaster.h"
#include "Camera.h"
#include "Model.h"
#include "Core/Window.h"


class Scene
{
public:
	Scene(Window* window, PhysicsEngine* physicsEngine);
	std::vector<std::unique_ptr<Light>>& getLights();
	std::vector<Light*> getShadowCastingLights();
	std::vector<std::unique_ptr<ShadowCaster>>& getShadowCasters();
	std::vector<std::unique_ptr<GameObject>>& getGameObjects();
	void createLight(glm::vec3 position, glm::vec4 colorAndType, bool castShadows = false);
	void update(float dt);
	glm::mat4* getCameraViewMatrix();
	glm::mat4* getCameraProjectionMatrix();
	Camera* getCamera();

private:
	std::unique_ptr<Camera> m_camera;
	std::vector<std::unique_ptr<GameObject>> m_gameObjects;
	std::vector<std::unique_ptr<Model>> m_models;
	std::vector<std::unique_ptr<Light>> m_lights;
	std::vector<std::unique_ptr<ShadowCaster>> m_shadowCasters;

	PhysicsEngine* m_physicsEngine;
	
	void addGameObject(std::unique_ptr<GameObject> gameObject);

	uint32_t m_gameObjectID = 0;
};

