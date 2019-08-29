#pragma once

#include "Entity.h"
#include "Model.h"

/* Forward declarations */
class VertexArray;
class Shader;
class IndexBuffer;

struct Camera
{
	glm::mat4 mViewProjection;
	glm::vec3 mPosition;
	glm::vec2 mAngles;
};

struct Light
{
	glm::mat4 mShadowSpace;
	glm::vec3 mLightDir;
};

struct Scene
{
	std::vector<Model>	mModels;
	Camera				mCamera;
	Light				mLight;
};

namespace Renderer
{
	void Init();
	void Clear();
	void Draw(Scene& scene, Shader shader, glm::mat4 viewProjection);
	void Draw(Mesh& Mesh);
	void Draw(const VertexArray& Va, const IndexBuffer& Ib);
	void Draw(const VertexArray& Va, unsigned int Count);
	void DrawInstanced(const VertexArray& Va, const IndexBuffer& Ib);
	void DrawInstanced(const VertexArray& Va, unsigned int Count);
};

