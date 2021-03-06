#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>


struct RenderTarget
{
	enum Attachment
	{
		Color,
		Depth,
		Count
	};
	GLuint		rendererID = 0;
	GLuint		attachments[Attachment::Count] {0};
	GLuint		textures[Attachment::Count] {0};
	glm::ivec2	size {0};
};

RenderTarget	makeRenderTarget(glm::ivec2 size, GLenum colodFormat, bool needsDepth);
RenderTarget	makeRenderTargetShadow(glm::ivec2 size);
RenderTarget	makeRenderTargetCube(glm::ivec2 size, bool needsDepth);
RenderTarget	makeRenderTargetMultisampled(glm::ivec2 size, GLenum colorFormat, int sampleCount);
void			setRenderTarget(RenderTarget*);
