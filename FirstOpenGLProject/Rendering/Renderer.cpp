#include "Renderer.h"

Renderer::Renderer()
{
    LoadShaders();
    m_shadowPass = std::make_unique<ShadowPass>(m_shaders["depthShader"], m_shaders["pointLightDepthShader"]);
    m_geometryPass = std::make_unique<GeometryPass>(m_shaders["deferredGeometryPass"]);
    m_lightingPass = std::make_unique<LightingPass>(m_shaders["deferredLightingPass"]);
}

void Renderer::update(Scene* scene) 
{
    m_shadowPass->execute(scene->getGameObjects(), scene->getShadowCastingLights(), scene->getShadowCasters());
    m_geometryPass->execute(scene->getGameObjects());
    m_lightingPass->execute(scene->getGameObjects());
}

void Renderer::LoadShaders() 
{
    m_shaders.emplace("shader", std::make_shared<Shader>("Shaders/shader.vert", "Shaders/shader.frag"));
    m_shaders.emplace("lightingShader", std::make_shared<Shader>("Shaders/lighting.vert", "Shaders/lighting.frag"));
    m_shaders.emplace("singleColorShader", std::make_shared<Shader>("Shaders/shader.vert", "Shaders/singleColor.frag"));
    m_shaders.emplace("kernelShader", std::make_shared<Shader>("Shaders/kernelShader.vert", "Shaders/kernelShader.frag"));
    m_shaders.emplace("depthShader", std::make_shared<Shader>("Shaders/depthShader.vert", "Shaders/depthShader.frag"));
    m_shaders.emplace("quadShader", std::make_shared<Shader>("Shaders/quad.vert", "Shaders/quad.frag"));
    m_shaders.emplace("pointLightDepthShader", std::make_shared<Shader>("Shaders/pointShadowDepthShader.vert", "Shaders/pointShadowDepthShader.geom", "Shaders/pointShadowDepthShader.frag"));
    m_shaders.emplace("bloomShader", std::make_shared<Shader>("Shaders/quad.vert", "Shaders/blur.frag"));
    m_shaders.emplace("deferredGeometryPass", std::make_shared<Shader>("Shaders/gBufferShader.vert", "Shaders/gBufferShader.frag"));
    m_shaders.emplace("deferredLightingPass", std::make_shared<Shader>("Shaders/quad.vert", "Shaders/gBufferLighting.frag"));
    m_shaders.emplace("ssaoShader", std::make_shared<Shader>("Shaders/quad.vert", "Shaders/ssao.frag"));
    m_shaders.emplace("ssaoBlurShader", std::make_shared<Shader>("Shaders/quad.vert", "Shaders/ssaoBlur.frag"));
}