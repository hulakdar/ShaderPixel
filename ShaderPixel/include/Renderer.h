#pragma once

#include "Model.h"

/* Forward declarations */
class VertexArray;
class Shader;
class IndexBuffer;

struct Camera
{
	glm::mat4 viewProjection;
	glm::vec3 position;
	glm::vec2 angles;
};

struct Light
{
	glm::mat4 shadowSpace;
	glm::vec3 lightDir;
};

struct Scene
{
	std::vector<Model>	models;
	Camera				camera;
	Light				light;
};

namespace Renderer
{
	void Init();
	void Clear();
	void Draw(Scene& scene, Shader& shader, glm::mat4 viewProjection);
	void Draw(Mesh* Mesh);
	void Draw(const VertexArray& Va, const IndexBuffer& Ib);
	void Draw(const VertexArray& Va, unsigned int Count);
	void DrawInstanced(const VertexArray& Va, const IndexBuffer& Ib);
	void DrawInstanced(const VertexArray& Va, unsigned int Count);
};

