#include "ShadowPass.h"

ShadowPass::ShadowPass(Shader* depthShader, Shader* pointLightDepthShader)
	: m_depthShader(depthShader), m_pointLightDepthShader(pointLightDepthShader)
{

}

void ShadowPass::execute(const std::vector<std::unique_ptr<GameObject>>&, const std::vector<std::unique_ptr<Light>>&)
{

}