#include "ShadowPass.h"

ShadowPass::ShadowPass(std::shared_ptr<Shader> depthShader, std::shared_ptr<Shader> pointLightDepthShader)
	: m_depthShader(depthShader), m_pointLightDepthShader(pointLightDepthShader)
{

}

void ShadowPass::execute(const std::vector<std::unique_ptr<GameObject>>& gameObjects, const std::vector<Light*>& lights, const std::vector<std::unique_ptr<ShadowCaster>>& shadowCasters)
{
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	for (size_t i = 0; i < lights.size(); i++) 
	{
		if (lights[i]->getLightType() == 1)//directional light 
		{
			
		}
		else if (lights[i]->getLightType() == 0)//spot light
		{
			m_pointLightDepthShader->use();
			int shadowCasterIndex = lights[i]->getShadowCastIndex();
			m_pointLightDepthShader->setMatrix4f("shadowMatrices", 6, shadowCasters[shadowCasterIndex]->getLightSpaceMatrix());
			m_pointLightDepthShader->setFloat("farPlane", shadowCasters[shadowCasterIndex]->getFarPlane());
			m_pointLightDepthShader->setVec3("lightPos", lights[i]->getPos());
			shadowCasters[shadowCasterIndex]->bindFBO();
			glClear(GL_DEPTH_BUFFER_BIT);
			for (size_t j = 0; j < lights.size(); j++) 
			{
				gameObjects[j]->renderDepth(*m_pointLightDepthShader);
			}
			shadowCasters[shadowCasterIndex]->unBindFBO();
		}
	}
	//reseting viewport and clearing screen
	//glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}