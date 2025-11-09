#include "OpenGLObjects/Texture2D.h"


Texture2D::Texture2D(GLenum internalFomrat, int width, int height, GLenum format, const void* pixels, GLenum type, float borderColor[4])
{
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture); 
	glTexImage2D(GL_TEXTURE_2D, 0, internalFomrat, width, height, 0, format == 0 ? internalFomrat : format, type, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (borderColor != 0) 
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	}
	else 
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int Texture2D::getID() { return m_texture; }

void Texture2D::bind() { glBindTexture(GL_TEXTURE_2D, m_texture); }
void Texture2D::unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

GLint Texture2D::getInternalFormat() 
{
	glBindTexture(GL_TEXTURE_2D, m_texture);
	GLint format;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
	return format;
}

void Texture2D::setTexWrapParametersi(GLenum tex_Wrap_S, GLenum tex_Wrap_T) 
{
	//calls glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex_Wrap_S)
	//calls glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex_Wrap_T)

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex_Wrap_S);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex_Wrap_S);
}

void Texture2D::setTexFilterParametersi(GLenum tex_Filter_Min, GLenum tex_Filter_Max)
{
	//calls glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tex_Wrap_S)
	//calls glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tex_Wrap_T)

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tex_Filter_Min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tex_Filter_Max);
}