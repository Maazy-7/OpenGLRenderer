#include "LightingPass.h"

LightingPass::LightingPass(Shader* shader)
	: m_lightingShader(shader)
{

}

void LightingPass::execute(const std::vector<std::unique_ptr<GameObject>>&) 
{

}