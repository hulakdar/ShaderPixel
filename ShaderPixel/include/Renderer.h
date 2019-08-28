#pragma once

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
	void DrawInstanced(const VertexArray& Va, const Shader& Program, const IndexBuffer& Ib);
	void DrawInstanced(const VertexArray& Va, const Shader& Program, unsigned int Count);
};

