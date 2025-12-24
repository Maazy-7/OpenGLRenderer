#include "Core/Application.h"

Application::Application() 
	: m_window(std::make_unique<Window>(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGLRenderer")),
	m_renderer(std::make_unique<Renderer>(SCREEN_WIDTH, SCREEN_HEIGHT)),
	m_physicsEngine(std::make_unique<PhysicsEngine>()),
	m_scene(std::make_unique<Scene>(m_window.get(),m_physicsEngine.get()))//should be readable from file or scenes should be managed using a scene manager
{

}

void Application::run() 
{
	while (!m_window->windowClose())
	{
		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)//temporary
			glfwSetWindowShouldClose(m_window->getWindow(), true);

		float dt = getDeltaTime();

		m_window->pollEvents();

		//input checker here

		m_scene->update(dt);

		m_physicsEngine->step(dt);

		m_renderer->update(m_scene.get());

		m_window->swapBuffers();
	}
}

float Application::getDeltaTime() 
{
	float currentTime = m_window->getTime();
	m_deltaTime = currentTime - m_prevTime;
	m_prevTime = currentTime;
	return m_deltaTime;
}