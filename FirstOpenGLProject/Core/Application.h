#pragma once

#include <iostream>
#include <memory>

#include "Core/Window.h"
#include "Physics/Core/PhysicsEngine.h"
#include "Rendering/Renderer.h"
#include "Core/Scene.h"
#include "settings.h"


class Application 
{

private:

	std::unique_ptr<Window> m_window;
	std::unique_ptr<Scene> m_scene;//should be able to support multiple scenes
	std::unique_ptr<Renderer> m_renderer;
	std::unique_ptr<PhysicsEngine> m_physicsEngine;
	float m_deltaTime = 0;

public:

	Application();
	void run();
	float getDeltaTime();
	

};