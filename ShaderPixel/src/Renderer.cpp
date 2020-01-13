#include "Renderer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "ShaderPixel.h"
#include "Utility.h"
#include "Resources.h"
#include "stb_image.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <iostream>
#include <algorithm>
#include <glm/ext/matrix_transform.hpp>
#include "imgui.h"

namespace Renderer
{
	RenderTarget viewport;

	static const float QuadData[] = {
		-1.f, -1.f, 0, 0,
		-1.f, 1.f,	0, 1,
		1.f, -1.f,	1, 0,
		1.f, 1.f,	1, 1,
	};

	static const float CubeData[] =
	{
		-10, -10, -10, 0.0f,0.0f, 0.0f,0.0f, -1.0f,
		 10,  10, -10, 1.0f,1.0f, 0.0f,0.0f, -1.0f,
		 10, -10, -10, 1.0f,0.0f, 0.0f,0.0f, -1.0f,
		-10,  10, -10, 0.0f,1.0f, 0.0f,0.0f, -1.0f,
		 10,  10, -10, 1.0f,1.0f, 0.0f,0.0f, -1.0f,
		-10, -10, -10, 0.0f,0.0f, 0.0f,0.0f, -1.0f,

		-10, -10,  10, 0.0f,0.0f, 0.0f,0.0f, 1.0f,
		 10, -10,  10, 1.0f,0.0f, 0.0f,0.0f, 1.0f,
		 10,  10,  10, 1.0f,1.0f, 0.0f,0.0f, 1.0f,
		 10,  10,  10, 1.0f,1.0f, 0.0f,0.0f, 1.0f,
		-10,  10,  10, 0.0f,1.0f, 0.0f,0.0f, 1.0f,
		-10, -10,  10, 0.0f,0.0f, 0.0f,0.0f, 1.0f,

		-10,  10,  10, 1.0f,0.0f, -1.0f,0.0f,0.0f,
		-10,  10, -10, 1.0f,1.0f, -1.0f,0.0f,0.0f,
		-10, -10, -10, 0.0f,1.0f, -1.0f,0.0f,0.0f,
		-10, -10, -10, 0.0f,1.0f, -1.0f,0.0f,0.0f,
		-10, -10,  10, 0.0f,0.0f, -1.0f,0.0f,0.0f,
		-10,  10,  10, 1.0f,0.0f, -1.0f,0.0f,0.0f,

		 10,  10,  10, 1.0f,0.0f, 1.0f,0.0f, 0.0f,
		 10, -10, -10, 0.0f,1.0f, 1.0f,0.0f, 0.0f,
		 10,  10, -10, 1.0f,1.0f, 1.0f,0.0f, 0.0f,
		 10, -10,  10, 0.0f,0.0f, 1.0f,0.0f, 0.0f,
		 10, -10, -10, 0.0f,1.0f, 1.0f,0.0f, 0.0f,
		 10,  10,  10, 1.0f,0.0f, 1.0f,0.0f, 0.0f,

		-10, -10, -10, 0.0f,1.0f, 0.0f,-1.0f,0.0f,
		 10, -10, -10, 1.0f,1.0f, 0.0f,-1.0f,0.0f,
		 10, -10,  10, 1.0f,0.0f, 0.0f,-1.0f,0.0f,
		 10, -10,  10, 1.0f,0.0f, 0.0f,-1.0f,0.0f,
		-10, -10,  10, 0.0f,0.0f, 0.0f,-1.0f,0.0f,
		-10, -10, -10, 0.0f,1.0f, 0.0f,-1.0f,0.0f,

		-10,  10, -10, 0.0f,1.0f, 0.0f,1.0f,0.0f,
		 10,  10,  10, 1.0f,0.0f, 0.0f,1.0f,0.0f,
		 10,  10, -10, 1.0f,1.0f, 0.0f,1.0f,0.0f,
		-10,  10,  10, 0.0f,0.0f, 0.0f,1.0f,0.0f,
		 10,  10,  10, 1.0f,0.0f, 0.0f,1.0f,0.0f,
		-10,  10, -10, 0.0f,1.0f, 0.0f,1.0f,0.0f
};

	static VertexArray	Quad;
	static VertexBuffer QuadBuffer;

	static VertexArray	Cube;
	static VertexBuffer CubeBuffer;

	void Init()
	{
		GLCall(glEnable(GL_DEPTH_TEST));
		stbi_set_flip_vertically_on_load(true);
		glEnable(GL_MULTISAMPLE); //need?

		assert(Resources::Textures.size() == 0);
		assert(Resources::Materials.size() == 0);

		Resources::Textures.emplace_back();// default texture ?
		Resources::Materials.emplace_back();// default material ?

		{
			QuadBuffer.Init(QuadData, sizeof(QuadData));

			VertexBufferLayout vbl;
			vbl.Push<float>(2);
			vbl.Push<float>(2);
			Quad.AddBuffer(QuadBuffer, vbl);
		}

		{
			CubeBuffer.Init(CubeData, sizeof(CubeData));
			VertexBufferLayout vbl;
			vbl.Push<float>(3);
			vbl.Push<float>(2);
			vbl.Push<float>(3);
			Cube.AddBuffer(CubeBuffer, vbl);
		}
	}

	void Update(int winX, int winY)
	{
		viewport.size = glm::ivec2(winX, winY);
	}

	void ApplyMaterial(Material* material, Shader* shader)
	{
		for (auto& It : material->uniforms)
		{
			shader->SetUniform(It);
		}
	}

	void Clear()
	{
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}

	void Draw(Scene& scene, glm::mat4 viewProjection, FeatureMask customFeatures /* = 0 */)
	{
		for (auto& model : scene.models)
		{
			glm::mat4 MVP = viewProjection * model.mModelSpace;
			for (auto& meshID : model.mMeshes)
			{
				Mesh* mesh = Resources::GetMesh(meshID);
				Material* material = Resources::GetMaterial(mesh->materialID);
				if (material)
				{
					if (material->blendMode == BlendMode::Opaque)
					{
						glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
						glEnable(GL_CULL_FACE);
					}
					else
					{
						glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
						glDisable(GL_CULL_FACE);
					}

					FeatureMask mask = material->features | customFeatures;

					ShaderID shaderID = Shader::GetShaderWithFeatures(mask);
					Shader* currentShader = Resources::GetShader(shaderID);

					if (currentShader)
					{
						ApplyMaterial(material, currentShader);
						currentShader->SetUniform("uMVP", MVP);
						currentShader->SetUniform("uModelToWorld", model.mModelSpace);
						currentShader->SetUniform("uEnvironment", 14); // hack. don't know how to set it properly
						currentShader->SetUniform("uShadow", 15); // hack. don't know how to set it properly
						Draw(mesh);
#ifdef DEVELOPMENT
						Draw(mesh->bounds, viewProjection);
#endif
					}
				}
			}
		}
		glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	}

	void Draw(Mesh* mesh)
	{
		if (mesh->count)
			Draw(mesh->vertexArray, mesh->count);
		else
			;//Draw(mesh->mVertexArray, mesh->mIndexBuffer);
	}

	void Draw(AABB& bounds, glm::mat4 viewProjection)
	{
		static unsigned char indexData[] = {
			0, 1, 2,
			0, 2, 3,
			2, 3, 4,
			2, 4, 5,
			4, 5, 6,
			4, 6, 7
		};

		if ((bounds.Min.x == bounds.Max.x) ||
			(bounds.Min.y == bounds.Max.y) ||
			(bounds.Min.z == bounds.Max.z))
			return;

		const glm::vec3 Scale =		(bounds.Max - bounds.Min) / 18.0f;
		const glm::vec3 Center =	(bounds.Max + bounds.Min) / 2.0f;

		ShaderID defaultShaderID = Shader::GetShaderWithFeatures(0);
		DrawCubeWS(Center, Scale, Resources::GetShader(defaultShaderID), viewProjection);
	}

	void Draw(const VertexArray& va, const IndexBuffer& ib)
	{
		va.Bind();
		ib.Bind();
		//GLCall(glDrawArrays(GL_TRIANGLES, 0, ib.GetCount() * 3));
		GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, 0));
	}

	void Draw(const VertexArray& va, unsigned int count)
	{
		va.Bind();
		GLCall(glDrawArrays(GL_TRIANGLES, 0, count));
	}

	void DrawQuadFS()
	{
		Quad.Bind();
		GLCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
	}

	void DrawQuadWS(glm::vec3 Position, glm::vec2 scale, Shader* shader, glm::mat4 viewProjection)
	{
		glDisable(GL_CULL_FACE);
		glm::mat4 modelSpace = glm::scale(glm::translate(glm::mat4(1), Position), glm::vec3(scale.x, scale.y, 1));
		glm::mat4 MVP = viewProjection * modelSpace;
		shader->SetUniform("uMVP", MVP);
		Quad.Bind();
		GLCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
	}

	void DrawCubeWS(glm::vec3 Position, float scale, Shader *shader, glm::mat4 viewProjection)
	{
		glm::mat4 modelSpace = glm::scale(glm::translate(glm::mat4(1), Position), glm::vec3(scale));
		glm::mat4 MVP = viewProjection * modelSpace;
		shader->SetUniform("uMVP", MVP);
		Cube.Bind();
		GLCall(glDrawArrays(GL_TRIANGLES, 0, ARRAY_COUNT(CubeData) / 8));
	}

	void DrawCubeWS(glm::vec3 Position, glm::vec3 scale, Shader *shader, glm::mat4 viewProjection)
	{
		glm::mat4 modelSpace = glm::scale(glm::translate(glm::mat4(1), Position), scale);
		glm::mat4 MVP = viewProjection * modelSpace;
		shader->SetUniform("uMVP", MVP);
		Cube.Bind();
		GLCall(glDrawArrays(GL_LINES, 0, ARRAY_COUNT(CubeData) / 8));
	}
}
