#include "Scene.h"

Scene::Scene(Window* window) 
{
//for now scene will be set and made in the constructor, will change to a scene manager system to load scenes from files
	m_camera = std::make_unique<Camera>(window->getWindow(), glm::vec3(0.f,0.f,3.f), window->getAspect());
	window->setWindowScrollCallBack(m_camera.get());


	//current scene with floor and a box and blender guy model on top of it

	//very messy Game Object Creation and manipulation, will refactor later
	m_models.push_back(std::make_unique<Model>("Assets/guy.obj"));
	m_gameObjects.push_back(std::make_unique<GameObject>(glm::vec3(-2.f, -0.3f, -4.f), m_models[0].get(), m_camera.get()));
	
	m_models.push_back(std::make_unique<Model>("Assets/brickThingy.obj"));
	m_gameObjects.push_back(std::make_unique<GameObject>(glm::vec3(2.f, 0.2f, 0.f), m_models[1].get(), m_camera.get()));
	m_gameObjects.push_back(std::make_unique<GameObject>(glm::vec3(0.f, -1.f, 0.f), m_models[1].get(), m_camera.get()));

	m_gameObjects[0]->rotate(glm::vec3(0.f, 0.f, -3.14f / 2.f));
	m_gameObjects[0]->scale(glm::vec3(0.4f));

	m_gameObjects[2]->setScale(glm::vec3(7.5f, 0.2f, 7.5f));
	m_gameObjects[2]->isStatic = true;
}

void Scene::update(float dt) 
{
	//currently not much but camera needs to be updated
	m_camera->update(dt);
}

glm::mat4* Scene::getCameraViewMatrix() 
{
	return m_camera->getView();
}

glm::mat4* Scene::getCameraProjectionMatrix() 
{
	return m_camera->getProjection();
}

std::vector<std::unique_ptr<Light>>& Scene::getLights() 
{
	return m_lights;
}

std::vector<Light*>& Scene::getShadowCastingLights()
{
	std::vector<Light*> shadowCastingLights;
	for (size_t i = 0; i < m_lights.size(); i++) 
	{
		if (m_lights[i]->castShadows()) { shadowCastingLights.push_back(m_lights[i].get()); }
	}
	return shadowCastingLights;
}

std::vector<std::unique_ptr<ShadowCaster>>& Scene::getShadowCasters() 
{
	return m_shadowCasters;
}

std::vector<std::unique_ptr<GameObject>>& Scene::getGameObjects() 
{
	return m_gameObjects;
}

void Scene::createLight(glm::vec3 position, glm::vec4 colorAndType, bool castShadows)
{
	if (castShadows) 
	{
		m_lights.push_back(std::make_unique<Light>(position, colorAndType, castShadows));
		int currentLightIndex = m_lights.size() - 1;
		m_shadowCasters.push_back(std::make_unique<ShadowCaster>(*m_lights[currentLightIndex]));
		int currentShadowCasterIndex = m_shadowCasters.size() - 1;
		m_lights[currentLightIndex]->setShadowCastIndex(currentShadowCasterIndex);
	}
	else
	{
		m_lights.push_back(std::make_unique<Light>(position, colorAndType, castShadows));
		int currentLightIndex = m_lights.size() - 1;
		m_lights[currentLightIndex]->setShadowCastIndex(-1);//noshadows
	}
}