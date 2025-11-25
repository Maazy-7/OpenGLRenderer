#pragma once

#include <glad/glad.h>

#include "Shader.h"

class UniformBuffer
{
public:
	UniformBuffer(GLint bindingPoint, GLsizeiptr size, const void* data, GLenum usage);
	void setShaderToUniform(Shader& shader, const GLchar* uniformName);
	void setUniformSubData(GLint offset, GLint size, const void* data);

private:
	unsigned int m_UBO;
	unsigned int m_bindingPoint;
};

