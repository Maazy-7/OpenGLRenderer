#include "Scene.h"

Scene::Scene(Window* window, PhysicsEngine* physicsEngine)
	: m_physicsEngine(physicsEngine)
{
//for now scene will be set and made in the constructor, will change to a scene manager system to load scenes from files
	m_camera = std::make_unique<Camera>(window->getWindow(), glm::vec3(0.f,0.f,3.f), window->getAspect());
	window->setWindowScrollCallBack(m_camera.get());


	//current scene with floor and a box and blender guy model on top of it

	//very messy Game Object Creation and manipulation, will refactor later
	m_models.push_back(std::make_unique<Model>("Assets/guy.obj"));
	m_models.push_back(std::make_unique<Model>("Assets/brickThingy.obj"));
	
	addGameObject(std::make_unique<GameObject>(std::make_unique<Transform>(glm::vec3(0.7f, 8.2f, 0.2f)), m_models[1].get(), m_camera.get()));

	addGameObject(std::make_unique<GameObject>(std::make_unique<Transform>(glm::vec3(2.f, -0.2f, 0.f)), m_models[1].get(), m_camera.get()));
	addGameObject(std::make_unique<GameObject>(std::make_unique<Transform>(glm::vec3(-0.3f, -2.3f, 0.f)), m_models[1].get(), m_camera.get()));

	m_gameObjects[1]->setStatic(true);
	m_gameObjects[2]->setScale(glm::vec3(7.5f, 1.2f, 7.5f));
	m_gameObjects[2]->setStatic(true);
	createLight(glm::vec3(3.f, 3.f, 1.f), glm::vec4(1.f,1.f,1.f,0.f)*3.f, true);
}

void Scene::update(float dt) 
{
	//currently not much but camera needs to be updated
	m_camera->update(dt);
	
	for (size_t i = 0; i < m_gameObjects.size(); i++) 
	{
		//m_gameObjects[i]->update(dt);
	}
}

Camera* Scene::getCamera() 
{
	return m_camera.get();
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

std::vector<Light*> Scene::getShadowCastingLights()
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

void Scene::addGameObject(std::unique_ptr<GameObject> gameObject) 
{
	m_gameObjects.push_back(std::move(gameObject));
	
	auto& currentObject = m_gameObjects.back();//the object which was just added to the gameobject list
	currentObject->setID(m_gameObjectID++);//unique GameObject ID, also sets Rigidbody ID

	if (currentObject->getRigidbody())
	{
		m_physicsEngine->addRigidBody(currentObject->getRigidbody());
	}
	if (currentObject->getCollider())
	{
		m_physicsEngine->addCollider(currentObject->getCollider());
	}
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