#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include "Shader.h"
#include "Rendering/ShadowPass.h"
#include "Rendering/GeometryPass.h"
#include "Rendering/LightingPass.h"
#include "Core/Scene.h"

class Renderer 
{
public:
	Renderer();
	void update(Scene* scene);
	std::shared_ptr<Shader> getShader(std::string name);

private:
	std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
	std::unique_ptr<ShadowPass> m_shadowPass;
	std::unique_ptr<GeometryPass> m_geometryPass;
	std::unique_ptr<LightingPass> m_lightingPass;
	void LoadShaders();
};