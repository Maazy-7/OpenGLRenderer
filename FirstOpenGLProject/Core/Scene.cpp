#include "Scene.h"

Scene::Scene(Window* window) 
{
//for now scene will be set and made in the constructor, will change to a scene manager system to load scenes from files
	m_camera = std::make_unique<Camera>(window->getWindow(), glm::vec3(0.f,0.f,3.f), window->getAspect());
	window->setWindowScrollCallBack(m_camera.get());


	//current scene with floor and a box and blender guy model on top of it

	//very messy Game Object Creation and manipulation, will refactor later
	m_models.push_back(std::make_unique<Model>("Assets/guy.obj"));
	//m_gameObjects.push_back(std::make_unique<GameObject>(glm::vec3(-2.f, -0.3f, -4.f), m_models[0].get(), m_camera.get()));
	
	m_models.push_back(std::make_unique<Model>("Assets/brickThingy.obj"));
	//m_gameObjects.push_back(std::make_unique<GameObject>(glm::vec3(2.f, 0.2f, 0.f), m_models[1].get(), m_camera.get()));
	//m_gameObjects.push_back(std::make_unique<GameObject>(glm::vec3(0.f, -1.f, 0.f), m_models[1].get(), m_camera.get()));

	//m_gameObjects[0]->rotate(glm::vec3(0.f, 0.f, -3.14f / 2.f));
	//m_gameObjects[0]->scale(glm::vec3(0.4f));

	//m_gameObjects[2]->setScale(glm::vec3(7.5f, 0.2f, 7.5f));
	//m_gameObjects[2]->isStatic = true;
}

/*
std::vector<std::unique_ptr<Light>> Scene::getLights() 
{
	return m_lights;
}

std::vector<std::unique_ptr<GameObject>> Scene::getGameObjects() 
{
	return m_gameObjects;
}*/

//TODO returning a vector of unique_ptr (the above functions^^^) causes an error