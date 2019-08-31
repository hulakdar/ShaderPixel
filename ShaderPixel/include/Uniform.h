#pragma once
#include "Texture.h"

#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

struct Uniform
{
	union {
		float			scalar;
		glm::vec2		v2;
		glm::vec3		v3;
		glm::vec4		v4;
		glm::ivec2		iv2;
		glm::ivec3		iv3;
		glm::ivec4		iv4;
		TextureBinding	tex;
	};
	enum {
		FLOAT,
		VEC2,
		VEC3,
		VEC4,
		IVEC2,
		IVEC3,
		IVEC4,
		TEX,
	} Type;
	std::string Name;
};
