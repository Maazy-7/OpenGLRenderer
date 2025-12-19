#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include "Shader.h"
#include "Rendering/ShadowPass.h"
#include "Rendering/GeometryPass.h"
#include "Rendering/LightingPass.h"
#include "Core/Scene.h"
#include "OpenGLObjects/Framebuffer.h"
#include "OpenGLObjects/Texture2D.h"
#include "OpenGLObjects/UniformBuffer.h"

class Renderer 
{
public:
	Renderer(int screenWidth, int screenHeight);
	void update(Scene* scene);
	std::shared_ptr<Shader> getShader(std::string name);
	void bindGBuffer();
	void unBindGBuffer();
	void bindGBufferTextures();
	void bindShadowMaps(Scene* scene);//currently only binds 1 shadow map

private:
	int m_screenWidth, m_screenHeight;
	std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
	std::unique_ptr<ShadowPass> m_shadowPass;
	std::unique_ptr<GeometryPass> m_geometryPass;
	std::unique_ptr<LightingPass> m_lightingPass;
	std::unique_ptr<Framebuffer> m_gBuffer;
	std::unique_ptr<Texture2D> m_gPosition;
	std::unique_ptr<Texture2D> m_gNormal;
	std::unique_ptr<Texture2D> m_gAlbedoSpec;
	std::unique_ptr<UniformBuffer> m_matricesUBO;
	void LoadShaders();
};