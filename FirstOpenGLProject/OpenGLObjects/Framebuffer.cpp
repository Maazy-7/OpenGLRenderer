#include "OpenGLObjects/Framebuffer.h"

Framebuffer::Framebuffer() 
{
	glGenFramebuffers(1, &m_FBO);
}

Framebuffer::Framebuffer(Texture2D tex, GLenum texAttachment)
{
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	attachTexture(tex, texAttachment);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


Framebuffer::Framebuffer(std::vector<Texture2D> textures, GLenum attachment)
{
	//one attachment type for all textures, but will have different number
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	attachTextures(textures, attachment);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::Framebuffer(std::vector<Texture2D> textures, std::vector<GLenum> texAttachments) 
{
	//Unique attachment for each texture
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	attachTextures(textures, texAttachments);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::Framebuffer(Cubemap tex)
{
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	attachTexture(tex);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::Framebuffer(Texture2D tex, GLenum texAttachment, GLenum renderBufferInternalFormat, GLenum renderBufferAttachment, int width, int height)
{
	//creates render buffer that attaches to the framebuffer
	glGenFramebuffers(1, &m_FBO);
	glGenRenderbuffers(1, &m_RBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
	
	glRenderbufferStorage(GL_RENDERBUFFER, renderBufferInternalFormat, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, renderBufferAttachment, GL_RENDERBUFFER, m_RBO);
	
	attachTexture(tex, texAttachment);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { std::cout << "Framebuffer not complete!" << std::endl; }

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::Framebuffer(std::vector<Texture2D> textures, GLenum texAttachment, GLenum renderBufferInternalFormat, GLenum renderBufferAttachment, int width, int height) 
{
	//creates render buffer that attaches to the framebuffer
	glGenFramebuffers(1, &m_FBO);
	glGenRenderbuffers(1, &m_RBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);

	glRenderbufferStorage(GL_RENDERBUFFER, renderBufferInternalFormat, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, renderBufferAttachment, GL_RENDERBUFFER, m_RBO);
	
	attachTextures(textures, texAttachment);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { std::cout << "Framebuffer not complete!" << std::endl; }

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::Framebuffer(std::vector<Texture2D> textures, std::vector<GLenum> texAttachments, GLenum renderBufferInternalFormat, GLenum renderBufferAttachment, int width, int height)
{
	//creates render buffer that attaches to the framebuffer
	glGenFramebuffers(1, &m_FBO);
	glGenRenderbuffers(1, &m_RBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);

	glRenderbufferStorage(GL_RENDERBUFFER, renderBufferInternalFormat, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, renderBufferAttachment, GL_RENDERBUFFER, m_RBO);

	attachTextures(textures, texAttachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { std::cout << "Framebuffer not complete!" << std::endl; }

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Framebuffer::attachTexture(Texture2D tex, GLenum texAttachment)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, texAttachment, GL_TEXTURE_2D, tex.getID(), 0);
	if (texAttachment == GL_DEPTH_ATTACHMENT) 
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::attachTextures(std::vector<Texture2D> textures, GLenum texAttachment) 
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	std::vector<GLenum> attachments;//data to send to glDrawBuffers

	for (int i = 0; i < textures.size(); i++) 
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, texAttachment + i, GL_TEXTURE_2D, textures[i].getID(), 0);
		attachments.push_back(texAttachment + i);
	}

	glDrawBuffers(textures.size(), attachments.data());//framebuffer will render to multiple textures

	if (texAttachment == GL_DEPTH_ATTACHMENT)
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::attachTextures(std::vector<Texture2D> textures, std::vector<GLenum> texAttachments)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	for (int i = 0; i < textures.size(); i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, texAttachments[i], GL_TEXTURE_2D, textures[i].getID(), 0);
	}

	glDrawBuffers(textures.size(), texAttachments.data());//framebuffer will render to multiple textures

	if (texAttachments[0] == GL_DEPTH_ATTACHMENT)
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::attachTexture(Cubemap tex)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tex.getID(), 0);//FOR NOW GL_DEPTH_ATTACHMENT, SHOULD BE CUSTOM LATER
	glDrawBuffer(GL_NONE);//read and draw buffers should also be customizable
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bind() { glBindFramebuffer(GL_FRAMEBUFFER, m_FBO); }
void Framebuffer::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

int Framebuffer::getID() { return m_FBO; }