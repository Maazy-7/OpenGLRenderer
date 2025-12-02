#pragma once

#include <iostream>
#include "glad/glad.h"
#include "OpenGLObjects/TextureBase.h"

class Cubemap : public TextureBase
{
	unsigned int m_cubeMap;

public:
	Cubemap(GLenum format, int width, int height);
	Cubemap(const char* path);
	~Cubemap();

	unsigned int getID() const override;
	void bind() const override;
	void unbind();
};