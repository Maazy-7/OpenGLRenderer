#include "ShadowCaster.h"

ShadowCaster::ShadowCaster(Light& light, float nearPlane, float farPlane)
	: m_nearPlane(nearPlane), m_farPlane(farPlane)
{
	if (light.getLightType() == 1) //directional light, 2D shadow map
	{
		float borderColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
		m_shadowMap = std::make_unique<Texture2D>(GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, GL_DEPTH_COMPONENT, (const void*)0, GL_FLOAT, borderColor);
		m_depthFBO = std::make_unique<Framebuffer>(m_shadowMap.get(), GL_DEPTH_ATTACHMENT);
		//setting light space matrix
		m_lightSpaceMatrix = new glm::mat4(glm::ortho(-20.f, 20.f, -20.f, 20.f, m_nearPlane, m_farPlane));
		*m_lightSpaceMatrix *= glm::lookAt(light.getPos(), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
		
	}
	else if (light.getLightType() == 0) //spot light, cubemap shadow
	{
		m_shadowMap = std::make_unique<Cubemap>(GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT);
		m_depthFBO = std::make_unique<Framebuffer>(m_shadowMap.get());
		//setting light space matrix
		glm::mat4 lightProjection = glm::perspective(glm::radians(90.f), (float)(SHADOW_WIDTH/SHADOW_HEIGHT), m_nearPlane, m_farPlane);
		m_lightSpaceMatrix = new glm::mat4[6];
		m_lightSpaceMatrix[0] = lightProjection * glm::lookAt(light.getPos(), light.getPos() + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		m_lightSpaceMatrix[1] = lightProjection * glm::lookAt(light.getPos(), light.getPos() + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		m_lightSpaceMatrix[2] = lightProjection * glm::lookAt(light.getPos(), light.getPos() + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		m_lightSpaceMatrix[3] = lightProjection * glm::lookAt(light.getPos(), light.getPos() + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		m_lightSpaceMatrix[4] = lightProjection * glm::lookAt(light.getPos(), light.getPos() + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		m_lightSpaceMatrix[5] = lightProjection * glm::lookAt(light.getPos(), light.getPos() + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	}
}

int ShadowCaster::getShadowWidth() { return SHADOW_WIDTH; }
int ShadowCaster::getShadowHeight() { return SHADOW_WIDTH; }
float ShadowCaster::getFarPlane() { return m_farPlane; }
float ShadowCaster::getNearPlane() { return m_nearPlane; }
void ShadowCaster::bindFBO() { m_depthFBO->bind(); }
void ShadowCaster::bindShadowMap() { m_shadowMap->bind(); }
void ShadowCaster::unBindFBO() { m_depthFBO->unbind(); }
glm::mat4* ShadowCaster::getLightSpaceMatrix() { return m_lightSpaceMatrix; }