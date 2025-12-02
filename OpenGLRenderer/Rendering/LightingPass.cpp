#include "LightingPass.h"

LightingPass::LightingPass(std::shared_ptr<Shader> shader)
	: m_lightingShader(shader), m_LightPassQuad(std::make_unique<QuadRenderer>())
{
	//setting gBuffer texture numbers
	m_lightingShader->use();
	m_lightingShader->setInt("gPosition", 0);
	m_lightingShader->setInt("gNormal", 1);
	m_lightingShader->setInt("gAlbedoSpec", 2);
	m_lightingShader->setInt("cubeDepthMap", 3);
	m_lightingShader->setInt("ssao", 4);
}

void LightingPass::execute(const std::vector<std::unique_ptr<Light>>& lights, Camera* camera) 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_lightingShader->use();
	m_lightingShader->setMatrix4f("viewMat", *camera->getView());
	m_lightingShader->setVec3("viewPos", camera->getPos());
	m_lightingShader->setInt("lightCount", lights.size());
	for (size_t i = 0; i < lights.size(); i++)
	{
		m_lightingShader->setVec3("lights[" + std::to_string(i) + "].position", lights[i]->getPos());
		m_lightingShader->setVec3("lights[" + std::to_string(i) + "].color", lights[i]->getColor());
		m_lightingShader->setFloat("lights[" + std::to_string(i) + "].linear", 0.09f);
		m_lightingShader->setFloat("lights[" + std::to_string(i) + "].quadratic", 0.32f);
	}
	//TODO need a way to bind depth maps for each light, and also allow for the shader to support more than one cubemap
	m_LightPassQuad->renderQuad();
}

void LightingPass::useLightingShader() { m_lightingShader->use(); }