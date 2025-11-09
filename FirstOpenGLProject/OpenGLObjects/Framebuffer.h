#pragma once

#include <glad/glad.h>
#include <iostream>
#include <vector>

#include "OpenGLObjects/Texture2D.h"
#include "OpenGLObjects/Cubemap.h"

class Framebuffer 
{

	unsigned int m_FBO, m_RBO;

public:

	Framebuffer();
	Framebuffer(Texture2D tex, GLenum texAttachment);
	Framebuffer(std::vector<Texture2D> textures, GLenum attachment);
	Framebuffer(std::vector<Texture2D> textures, std::vector<GLenum> texAttachments);

	Framebuffer(Texture2D tex, GLenum texAttachment, GLenum renderBufferInternalFormat, GLenum renderBufferAttachment, int width = 800, int height = 600);
	Framebuffer(std::vector<Texture2D> textures, GLenum texAttachment, GLenum renderBufferInternalFormat, GLenum renderBufferAttachment, int width = 800, int height = 600);
	Framebuffer(std::vector<Texture2D> textures, std::vector<GLenum> texAttachments, GLenum renderBufferInternalFormat, GLenum renderBufferAttachment, int width = 800, int height = 600);
	Framebuffer(Cubemap tex);

	void attachTexture(Texture2D tex, GLenum texAttachment);
	void attachTextures(std::vector<Texture2D> textures, GLenum texAttachment);
	void attachTextures(std::vector<Texture2D> textures, std::vector<GLenum> texAttachments);
	void attachTexture(Cubemap tex);
	void attachRenderbuffer();

	void bind();
	void unbind();

	int getID();

};