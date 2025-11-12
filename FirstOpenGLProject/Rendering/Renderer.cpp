#include "Renderer.h"

Renderer::Renderer()
{
    LoadShaders();
    m_shadowPass = std::make_unique<ShadowPass>(m_shaders["depthShader"].get(), m_shaders["pointLightDepthShader"].get());
    m_geometryPass = std::make_unique<GeometryPass>(m_shaders["deferredGeometryPass"].get());
    m_lightingPass = std::make_unique<LightingPass>(m_shaders["deferredLightingPass"].get());
}

void Renderer::update(Scene* scene) 
{
    //m_shadowPass->execute(scene->getGameObjects(), scene->getLights());
    //m_geometryPass->execute(scene->getGameObjects());
    //m_lightingPass->execute(scene->getGameObjects());
}

void Renderer::LoadShaders() 
{
    m_shaders.emplace("shader", std::make_unique<Shader>("Shaders/shader.vert", "Shaders/shader.frag"));
    m_shaders.emplace("lightingShader", std::make_unique<Shader>("Shaders/lighting.vert", "Shaders/lighting.frag"));
    m_shaders.emplace("singleColorShader", std::make_unique<Shader>("Shaders/shader.vert", "Shaders/singleColor.frag"));
    m_shaders.emplace("kernelShader", std::make_unique<Shader>("Shaders/kernelShader.vert", "Shaders/kernelShader.frag"));
    m_shaders.emplace("depthShader", std::make_unique<Shader>("Shaders/depthShader.vert", "Shaders/depthShader.frag"));
    m_shaders.emplace("quadShader", std::make_unique<Shader>("Shaders/quad.vert", "Shaders/quad.frag"));
    m_shaders.emplace("pointLightDepthShader", std::make_unique<Shader>("Shaders/pointShadowDepthShader.vert", "Shaders/pointShadowDepthShader.geom", "Shaders/pointShadowDepthShader.frag"));
    m_shaders.emplace("bloomShader", std::make_unique<Shader>("Shaders/quad.vert", "Shaders/blur.frag"));
    m_shaders.emplace("deferredGeometryPass", std::make_unique<Shader>("Shaders/gBufferShader.vert", "Shaders/gBufferShader.frag"));
    m_shaders.emplace("deferredLightingPass", std::make_unique<Shader>("Shaders/quad.vert", "Shaders/gBufferLighting.frag"));
    m_shaders.emplace("ssaoShader", std::make_unique<Shader>("Shaders/quad.vert", "Shaders/ssao.frag"));
    m_shaders.emplace("ssaoBlurShader", std::make_unique<Shader>("Shaders/quad.vert", "Shaders/ssaoBlur.frag"));
}