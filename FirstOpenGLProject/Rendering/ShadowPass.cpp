#include "ShadowPass.h"

ShadowPass::ShadowPass(std::shared_ptr<Shader> depthShader, std::shared_ptr<Shader> pointLightDepthShader)
	: m_depthShader(depthShader), m_pointLightDepthShader(pointLightDepthShader)
{

}

void ShadowPass::execute(const std::vector<std::unique_ptr<GameObject>>& gameObjects, const std::vector<Light*>& lights, const std::vector<std::unique_ptr<ShadowCaster>>& shadowCasters)
{
	//way to store window width and height to reset viewport later
	int dimensions[4];
	glGetIntegerv(GL_VIEWPORT, dimensions);
	int screenWidth = dimensions[2];
	int screenHeight = dimensions[3];

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	for (size_t i = 0; i < lights.size(); i++) 
	{
		if (lights[i]->getLightType() == 1)//directional light 
		{
			m_depthShader->use();
			int shadowCasterIndex = lights[i]->getShadowCastIndex();
			m_depthShader->setMatrix4f("lightSpaceMatrix", *shadowCasters[shadowCasterIndex]->getLightSpaceMatrix());
			shadowCasters[shadowCasterIndex]->bindFBO();
			glClear(GL_DEPTH_BUFFER_BIT);
			for (size_t j = 0; j < lights.size(); j++) 
			{
				gameObjects[j]->renderDepth(*m_depthShader);
			}
			shadowCasters[shadowCasterIndex]->unBindFBO();

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
	glViewport(0, 0, screenWidth, screenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}