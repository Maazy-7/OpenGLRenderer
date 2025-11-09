#pragma once

#include <glad/glad.h>

class Texture2D 
{
	unsigned int m_texture;

public:
	
	Texture2D(GLenum internalFormat, int width, int height, GLenum format = 0, const void* pixels = 0, GLenum type = GL_FLOAT, float borderColor[4] = { 0 });
	Texture2D(const char* path);
	unsigned int getID();
	void bind();
	void unbind();
	void setTexWrapParametersi(GLenum tex_Wrap_S, GLenum tex_Wrap_T);
	void setTexFilterParametersi(GLenum tex_Filter_Min, GLenum tex_Filter_Max);
	GLint getInternalFormat();
};