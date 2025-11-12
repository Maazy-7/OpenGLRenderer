#pragma once

#include "OpenGLObjects/Framebuffer.h"
#include "OpenGLObjects/Texture2D.h"
#include "OpenGLObjects/Cubemap.h"
#include "Rendering/Light.h"


class ShadowCaster
{
public:
	ShadowCaster(Light* light);

private:
	int SHADOW_WIDTH = 1024;
	int SHADOW_HEIGHT = 1024;
	unsigned int m_shadowMap;//will get cast to either a Cubemap or Texture2D object since they are both just wrappers around an unsigned int
};

