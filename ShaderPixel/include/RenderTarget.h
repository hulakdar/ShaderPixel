#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>


struct RenderTarget
{
	GLuint		rendererID;
	glm::ivec2	size;
};
