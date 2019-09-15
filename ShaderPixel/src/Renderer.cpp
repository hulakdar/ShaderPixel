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
	static const float QuadData[] = {
		-1.f, 1.f,	0, 1,
		-1.f, -1.f, 0, 0,
		
		1.f, -1.f,	1, 0,
		1.f, -1.f,	1, 0,

		1.f, 1.f,	1, 1,
		-1.f, 1.f,	0, 1,
	};

	static const float CubeData[] =
	{
		// ordering is not consistent. this causes culling issues. need to reorder
		-10, -10, -10, 0.0f,0.0f, 0.0f,0.0f, -1.0f,
		 10, -10, -10, 1.0f,0.0f, 0.0f,0.0f, -1.0f,
		 10,  10, -10, 1.0f,1.0f, 0.0f,0.0f, -1.0f,
		 10,  10, -10, 1.0f,1.0f, 0.0f,0.0f, -1.0f,
		-10,  10, -10, 0.0f,1.0f, 0.0f,0.0f, -1.0f,
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
		 10,  10, -10, 1.0f,1.0f, 1.0f,0.0f, 0.0f,
		 10, -10, -10, 0.0f,1.0f, 1.0f,0.0f, 0.0f,
		 10, -10, -10, 0.0f,1.0f, 1.0f,0.0f, 0.0f,
		 10, -10,  10, 0.0f,0.0f, 1.0f,0.0f, 0.0f,
		 10,  10,  10, 1.0f,0.0f, 1.0f,0.0f, 0.0f,

		-10, -10, -10, 0.0f,1.0f, 0.0f,-1.0f,0.0f,
		 10, -10, -10, 1.0f,1.0f, 0.0f,-1.0f,0.0f,
		 10, -10,  10, 1.0f,0.0f, 0.0f,-1.0f,0.0f,
		 10, -10,  10, 1.0f,0.0f, 0.0f,-1.0f,0.0f,
		-10, -10,  10, 0.0f,0.0f, 0.0f,-1.0f,0.0f,
		-10, -10, -10, 0.0f,1.0f, 0.0f,-1.0f,0.0f,

		-10,  10, -10, 0.0f,1.0f, 0.0f,1.0f,0.0f,
		 10,  10, -10, 1.0f,1.0f, 0.0f,1.0f,0.0f,
		 10,  10,  10, 1.0f,0.0f, 0.0f,1.0f,0.0f,
		 10,  10,  10, 1.0f,0.0f, 0.0f,1.0f,0.0f,
		-10,  10,  10, 0.0f,0.0f, 0.0f,1.0f,0.0f,
		-10,  10, -10, 0.0f,1.0f, 0.0f,1.0f,0.0f
};

	static VertexArray	Quad;
	static VertexBuffer QuadBuffer;

	static VertexArray	Cube;
	static VertexBuffer CubeBuffer;

	static ShaderID	FXAAID;
	static ShaderID	MandelbrotID;

	void Init()
	{
		GLCall(glEnable(GL_DEPTH_TEST));
		stbi_set_flip_vertically_on_load(true);
		//glEnable(GL_MULTISAMPLE); need?

		assert(Resources::Shaders.size() == 0);
		assert(Resources::Textures.size() == 0);
		Resources::Shaders.emplace_back(0); // default shader
		Resources::Textures.emplace_back();// default texture

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

		FXAAID = Resources::Shaders.size();
		Resources::Shaders.emplace_back(
		"../content/shaders/vertFXAA.shader",
		"../content/shaders/fragFXAA.shader");
	

		MandelbrotID = Resources::Shaders.size();
		Resources::Shaders.emplace_back(
		"../content/shaders/vertWorldSpace.shader",
		"../content/shaders/fragMandelbrot.shader");
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

	void Draw(Scene& scene, Shader* inputShader, glm::mat4 viewProjection)
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
						glEnable(GL_CULL_FACE);
						glCullFace(GL_BACK);
					}
					else
					{
						glDisable(GL_CULL_FACE);
					}

					Shader* currentShader = inputShader;

					// cache this? sort by this?
					if (material->shaderOverride)
						currentShader = Resources::GetShader(material->shaderOverride);

					// just in case.  this should never trigger
					if (!currentShader)
						currentShader = inputShader;

					if (currentShader)
					{
						ApplyMaterial(material, currentShader);
						currentShader->SetUniform("uMVP", MVP);
						Draw(mesh);
					}
				}
			}
		}
	}

	void Draw(Mesh* mesh)
	{
		if (mesh->count)
			Draw(mesh->vertexArray, mesh->count);
		else
			;//Draw(mesh->mVertexArray, mesh->mIndexBuffer);
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

	void DrawInstanced(const VertexArray& va, const IndexBuffer& ib)
	{
		va.Bind();
		ib.Bind();
		std::cerr << "Instanced is not implemented yet \n";
		//GLCall(glDrawElementsInstanced(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, 0));
	}

	void DrawInstanced(const VertexArray& va, unsigned int count)
	{
		va.Bind();
		std::cerr << "Instanced is not implemented yet \n";
		//GLCall(glDrawArraysInstanced(GL_TRIANGLES, 0, Count));
	}

	void DrawQuadFS()
	{
		Quad.Bind();
		GLCall(glDrawArrays(GL_TRIANGLES, 0, ARRAY_COUNT(QuadData)));
	}

	void DrawQuadWS(glm::vec3 Position, glm::vec2 scale, Shader* shader, glm::mat4 viewProjection)
	{
		glDisable(GL_CULL_FACE);
		glm::mat4 modelSpace = glm::scale(glm::translate(glm::mat4(1), Position), glm::vec3(scale.x, scale.y, 1));
		glm::mat4 MVP = viewProjection * modelSpace;
		shader->SetUniform("uMVP", MVP);
		Quad.Bind();
		GLCall(glDrawArrays(GL_TRIANGLES, 0, ARRAY_COUNT(QuadData)));
	}

	void DrawCubeWS(glm::vec3 Position, float scale, Shader *shader, glm::mat4 viewProjection)
	{
		glm::mat4 modelSpace = glm::scale(glm::translate(glm::mat4(1), Position), glm::vec3(scale));
		glm::mat4 MVP = viewProjection * modelSpace;
		shader->SetUniform("uMVP", MVP);
		Cube.Bind();
		GLCall(glDrawArrays(GL_TRIANGLES, 0, ARRAY_COUNT(CubeData)));
	}

	void DrawMandelbrot(glm::vec3 cameraPosition, glm::mat4 viewProjection)
	{
		Shader *mandel = Resources::GetShader(MandelbrotID);

		static glm::vec3 QuadPos{ -50,140,-263 };
		static glm::vec2 QuadScale{ 145,111 };
		static glm::vec2 QuadPos2 = glm::vec2(QuadPos.x, QuadPos.z);

		static glm::vec2 MandelPos{ 0,0 };
		static float MandelScale{ 2 };
		static float MandelIter{ 20 };

		glm::vec2 cameraPos2 = glm::vec2(cameraPosition.x, cameraPosition.z);
		if (glm::length(cameraPos2 - QuadPos2) < 150)
		{
			ImGui::DragFloat2("MandelPos", &MandelPos[0], 0.001f);
			ImGui::DragFloat("MandelScale", &MandelScale, 0.0001f);
			ImGui::DragFloat("MandelIter", &MandelIter);
		}
		mandel->Bind();
		mandel->SetUniform("uCenter", MandelPos);
		mandel->SetUniform("uScale", MandelScale);
		mandel->SetUniform("uIter", MandelIter);

		Renderer::DrawQuadWS(QuadPos, QuadScale, mandel, viewProjection);
	}
}
