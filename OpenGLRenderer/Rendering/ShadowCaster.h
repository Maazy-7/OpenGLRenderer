#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "OpenGLObjects/TextureBase.h"
#include "OpenGLObjects/Framebuffer.h"
#include "OpenGLObjects/Texture2D.h"
#include "OpenGLObjects/Cubemap.h"
#include "Rendering/Light.h"


class ShadowCaster
{
public:
	ShadowCaster(Light& light, float nearPlane = 0.1f, float farPlane = 25.f);
	int getShadowWidth();
	int getShadowHeight();
	float getFarPlane();
	float getNearPlane();
	void bindFBO();
	void bindShadowMap();
	void unBindFBO();
	glm::mat4* getLightSpaceMatrix();

private:
	int SHADOW_WIDTH = 1024;
	int SHADOW_HEIGHT = 1024;
	float m_nearPlane;
	float m_farPlane;
	std::unique_ptr<TextureBase> m_shadowMap;//will be either a Cubemap or Texture2D
	std::unique_ptr<Framebuffer> m_depthFBO;
	glm::mat4* m_lightSpaceMatrix = nullptr;
};

