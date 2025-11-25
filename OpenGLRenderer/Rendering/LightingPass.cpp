#include "LightingPass.h"

LightingPass::LightingPass(std::shared_ptr<Shader> shader)
	: m_lightingShader(shader)
{
	//setting gBuffer texture numbers
	m_lightingShader->use();
	m_lightingShader->setInt("gPosition", 0);
	m_lightingShader->setInt("gNormal", 1);
	m_lightingShader->setInt("gAlbedoSpec", 2);
}

void LightingPass::execute(const std::vector<std::unique_ptr<GameObject>>&) 
{
	
}