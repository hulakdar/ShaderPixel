#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Entity.h"

/* Forward declarations */
class VertexArray;
class Shader;
class IndexBuffer;

namespace Renderer
{
	void Init();
	void Clear();
	void Draw(const VertexArray& Va, const Shader& Program, const IndexBuffer& Ib);
	void Draw(const VertexArray& Va, const Shader& Program, unsigned int Count);
};

