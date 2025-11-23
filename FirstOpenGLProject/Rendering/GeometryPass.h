#pragma once

#include <vector>
#include <memory>

#include "Shader.h"
#include "Physics/Components/GameObject.h"
#include "OpenGLObjects/Framebuffer.h"
#include "OpenGLObjects/Texture2D.h"

class GeometryPass
{
public:
	GeometryPass(std::shared_ptr<Shader> shader);
	void execute(const std::vector<std::unique_ptr<GameObject>>& gameObjects);

private:
	std::shared_ptr<Shader> m_geometryShader;
};

