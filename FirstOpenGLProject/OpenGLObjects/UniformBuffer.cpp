#include "UniformBuffer.h"


UniformBuffer::UniformBuffer(GLint bindingPoint, GLsizeiptr size, const void* data, GLenum usage)
	:m_bindingPoint(bindingPoint)
{
	glGenBuffers(1, &m_UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
	glBufferData(GL_UNIFORM_BUFFER, size, data, usage);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, m_UBO, 0, size);//setting range to [0 - sizeof data]
}

void UniformBuffer::setShaderToUniform(Shader& shader, const GLchar* uniformName) 
{
	unsigned int index = glGetUniformBlockIndex(shader.m_ID, uniformName);
	glUniformBlockBinding(shader.m_ID, index, m_bindingPoint);
}

void UniformBuffer::setUniformSubData(GLint offset, GLint size, const void* data)
{
	glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}