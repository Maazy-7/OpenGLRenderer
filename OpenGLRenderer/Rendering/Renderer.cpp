#include "Renderer.h"

Renderer::Renderer(int screenWidth, int screenHeight)
    : m_screenWidth(screenWidth), m_screenHeight(screenHeight)
{
    LoadShaders();
    m_shadowPass = std::make_unique<ShadowPass>(m_shaders["depthShader"], m_shaders["pointLightDepthShader"]);
    m_geometryPass = std::make_unique<GeometryPass>(m_shaders["deferredGeometryPass"]);
    m_lightingPass = std::make_unique<LightingPass>(m_shaders["deferredLightingPass"]);

    //matrices uniform buffer
    m_matricesUBO = std::make_unique<UniformBuffer>(0, 2 * sizeof(glm::mat4), (const void*)0, GL_DYNAMIC_DRAW);
    m_matricesUBO->setShaderToUniform(*m_shaders["deferredGeometryPass"], "Matrices");

    //set up gBuffer
    m_gPosition = std::make_unique<Texture2D>(GL_RGBA16F, m_screenWidth, m_screenHeight, GL_RGBA); //(x,y,z) position
    m_gPosition->setTexFilterParametersi(GL_NEAREST, GL_NEAREST);
    m_gNormal = std::make_unique<Texture2D>(GL_RGBA16F, m_screenWidth, m_screenHeight, GL_RGBA); //(x,y,z) normal
    m_gNormal->setTexFilterParametersi(GL_NEAREST, GL_NEAREST);
    m_gAlbedoSpec = std::make_unique<Texture2D>(GL_RGBA, m_screenWidth, m_screenHeight, GL_RGBA, (const void*)0, GL_UNSIGNED_BYTE); //(r,g,b, a) diffuse and specular
    
    //temporary lists since since std::make_unique cant pass initializer lists to framebuffer constructor, should modify framebuffer constructor
    std::vector<Texture2D*> list = { m_gPosition.get(), m_gNormal.get(), m_gAlbedoSpec.get()};
    std::vector<GLenum> attachments = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

    m_gBuffer = std::make_unique<Framebuffer>(list, attachments, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, screenWidth, screenHeight);

    glEnable(GL_DEPTH_TEST);
}

void Renderer::update(Scene* scene) 
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    //setting Matrices uniform buffer object data
    m_matricesUBO->setUniformSubData(0, sizeof(glm::mat4), scene->getCameraViewMatrix());
    m_matricesUBO->setUniformSubData(sizeof(glm::mat4), sizeof(glm::mat4), scene->getCameraProjectionMatrix());

    //executing render passes
    m_shadowPass->execute(scene->getGameObjects(), scene->getShadowCastingLights(), scene->getShadowCasters());
    
    bindGBuffer();
    m_geometryPass->execute(scene->getGameObjects());
    unBindGBuffer();
    
    m_lightingPass->useLightingShader();
    bindGBufferTextures();
    glActiveTexture(GL_TEXTURE3);
    /*temp*/scene->getShadowCasters()[0]->bindShadowMap();
    m_lightingPass->execute(scene->getLights(), scene->getCamera());
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

std::shared_ptr<Shader> Renderer::getShader(std::string name) 
{
    if (m_shaders.find(name) != m_shaders.end()) { return m_shaders.at(name); }
    else { std::cout << "shader does not exist\n"; return nullptr; }
}

void Renderer::bindGBuffer() 
{
    m_gBuffer->bind();
}

void Renderer::unBindGBuffer() 
{
    m_gBuffer->unbind();
}

void Renderer::bindGBufferTextures() 
{
    //texture numbers set during deferred lighting shader creation
    glActiveTexture(GL_TEXTURE0);
    m_gPosition->bind();
    glActiveTexture(GL_TEXTURE1);
    m_gNormal->bind();
    glActiveTexture(GL_TEXTURE2);
    m_gAlbedoSpec->bind();
}