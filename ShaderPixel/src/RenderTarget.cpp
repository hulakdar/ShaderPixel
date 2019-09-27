#include "RenderTarget.h"
#include "Utility.h"

RenderTarget makeRenderTargetMultisampled(glm::ivec2 size, GLenum colorFormat, int sampleCount)
{
	RenderTarget result;

	result.size = size;

	// color
	{
		GLCall(glGenFramebuffers(1, &result.rendererID));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, result.rendererID));

		GLCall(glGenRenderbuffers(2, result.attachments));

		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, result.attachments[RenderTarget::Color]));
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampleCount, colorFormat, size.x, size.y);
		GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, result.attachments[RenderTarget::Color]));
	}

	// depth
	{
		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, result.attachments[RenderTarget::Depth]));
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampleCount, GL_DEPTH_COMPONENT, size.x, size.y);
		GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, result.attachments[RenderTarget::Depth]));
	}

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		__debugbreak();

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	return result;
}

RenderTarget makeRenderTargetShadow(glm::ivec2 size)
{
	RenderTarget result;

	result.size = size;

	GLCall(glGenFramebuffers(1, &result.rendererID));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, result.rendererID));

	GLCall(glGenRenderbuffers(1, &result.attachments[RenderTarget::Depth]));

	GLCall(glGenTextures(1, &result.textures[RenderTarget::Depth]));
	GLCall(glBindTexture(GL_TEXTURE_2D, result.textures[RenderTarget::Depth]));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_GREATER));

	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, result.textures[RenderTarget::Depth], 0));

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		__debugbreak();

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	return result;
}
RenderTarget makeRenderTarget(glm::ivec2 size, GLenum colorFormat, bool needsDepth)
{
	RenderTarget result;

	result.size = size;

	GLCall(glGenFramebuffers(1, &result.rendererID));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, result.rendererID));

	GLCall(glGenRenderbuffers(1 + needsDepth, result.attachments));

	GLCall(glGenTextures(1 + needsDepth, &result.textures[RenderTarget::Color]));
	GLCall(glBindTexture(GL_TEXTURE_2D, result.textures[RenderTarget::Color]));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, size.x, size.y, 0, colorFormat, GL_UNSIGNED_BYTE, 0));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	//GLCall(glBindRenderbuffer(GL_RENDERBUFFER, result.attachments[RenderTarget::Color]));
	//GLCall(glRenderbufferStorage(GL_RENDERBUFFER, colorFormat, size.x, size.y));
	//glRenderbufferStorageMultisample(GL_RENDERBUFFER, 16, colorFormat, size.x, size.y);

	//GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, result.attachments[RenderTarget::Color]));
	GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, result.textures[RenderTarget::Color], 0));
	//GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, result.textures[RenderTarget::Color], 0));
	//GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));

	if (needsDepth)
	{
		GLCall(glBindTexture(GL_TEXTURE_2D, result.textures[RenderTarget::Depth]));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, result.attachments[RenderTarget::Depth]));
		//GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y));
		//glRenderbufferStorageMultisample(GL_RENDERBUFFER, 16, GL_DEPTH_COMPONENT, size.x, size.y);

		//GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, result.attachments[RenderTarget::Depth]));
		//GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, result.textures[RenderTarget::Color], 0));
		GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, result.textures[RenderTarget::Depth], 0));
		//GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
	}

	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0};// , GL_DEPTH_ATTACHMENT 
	GLCall(glDrawBuffers(ARRAY_COUNT(DrawBuffers), DrawBuffers));

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		__debugbreak();

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	return result;
}

void setRenderTarget(RenderTarget* rt)
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, rt->rendererID));
	GLCall(glViewport(0,0, rt->size.x, rt->size.y));
}
