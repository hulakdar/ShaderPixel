#pragma once
#include "Model.h"
#include "RenderTarget.h"

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
	extern RenderTarget viewport;

	void Init();
	void Update(int winX, int winY);
	void Clear();
	void Draw(Scene& scene, glm::mat4 viewProjection, FeatureMask features = 0);
	void Draw(Mesh* Mesh);
	void Draw(const VertexArray& Va, const IndexBuffer& Ib);
	void Draw(const VertexArray& Va, unsigned int Count);
	void Draw(AABB& bounds);
	void DrawQuadFS();
	void DrawQuadWS(glm::vec3 Position, glm::vec2 scale, Shader* shader, glm::mat4 viewProjection);
	void DrawCubeWS(glm::vec3 Position, float scale, Shader* shader, glm::mat4 viewProjection);
};

