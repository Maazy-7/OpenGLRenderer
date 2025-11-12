#include "Core/Application.h"

Application::Application() 
	: m_window(std::make_unique<Window>(SCREEN_WIDTH, SCREEN_HEIGHT, "Renderer")),
	m_scene(std::make_unique<Scene>(m_window.get())),//should be readable from file or scenes should be managed using a scene manager
	m_renderer(std::make_unique<Renderer>()),
	m_physicsEngine(std::make_unique<PhysicsEngine>())
{

}

void Application::run() 
{
	float dt = getDeltaTime();

	m_window->pollEvents();

	//input checker here

	m_physicsEngine->step(dt);

	m_renderer->update(m_scene.get());

	m_window->swapBuffers();
}

float Application::getDeltaTime() 
{
	return m_deltaTime = m_window->getTime() - m_deltaTime;
}