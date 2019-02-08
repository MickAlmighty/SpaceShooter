#include <Framebuffer.h>
#include <glad/glad.h>
#include <iostream>

Framebuffer::Framebuffer(const unsigned int SHADOW_WIDTH, const unsigned int SHADOW_HEIGHT)
{

	framebufferHeight = SHADOW_HEIGHT;
	framebufferWidth = SHADOW_WIDTH;
	glGenFramebuffers(1, &framebufferID);
	glGenTextures(1, &framebufferTextureID);
}

Framebuffer::~Framebuffer()
{
	glDeleteTextures(1, &framebufferTextureID);
	glDeleteFramebuffers(1, &framebufferID);
}

void Framebuffer::InitFramebuffer()
{
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer status error!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::GenerateTexture2D()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	glBindTexture(GL_TEXTURE_2D, framebufferTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		framebufferWidth, framebufferHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, framebufferTextureID, 0);
}

void Framebuffer::GenerateCubemapTexture()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, framebufferTextureID);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			framebufferWidth, framebufferHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, framebufferTextureID, 0);
}

unsigned int Framebuffer::GetFramebufferID()
{
	return framebufferID;
}

unsigned int Framebuffer::GetFramebufferTextureID()
{
	return framebufferTextureID;
}

unsigned int Framebuffer::GetWidth()
{
	return framebufferWidth;
}

unsigned int Framebuffer::GetHeight()
{
	return framebufferHeight;
}