#pragma once

#include "glad/glad.h"

class Cubemap 
{
	unsigned int m_cubeMap;

public:
	Cubemap(GLenum format, int width, int height);
	Cubemap(const char* path);

	unsigned int getID();
	void bind();
	void unbind();
};