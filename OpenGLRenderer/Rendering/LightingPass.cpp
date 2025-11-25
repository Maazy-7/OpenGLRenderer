#include "LightingPass.h"

LightingPass::LightingPass(std::shared_ptr<Shader> shader)
	: m_lightingShader(shader)
{

}

void LightingPass::execute(const std::vector<std::unique_ptr<GameObject>>&) 
{
	
}