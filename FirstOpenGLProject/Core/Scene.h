#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "Physics/Components/GameObject.h"
#include "Rendering/Light.h"
#include "Rendering/ShadowCaster.h"
#include "Camera.h"
#include "Model.h"
#include "Core/Window.h"


class Scene
{
public:
	Scene(Window* window);
	//std::vector<std::unique_ptr<Light>> getLights();
	//std::vector<std::unique_ptr<GameObject>> getGameObjects();

private:
	std::unique_ptr<Camera> m_camera;
	std::vector<std::unique_ptr<GameObject>> m_gameObjects;
	std::vector<std::unique_ptr<Model>> m_models;
	std::vector<std::unique_ptr<Light>> m_lights;
	std::vector<std::unique_ptr<ShadowCaster>> m_shadowCasters;
};

