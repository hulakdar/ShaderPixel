#include "Renderer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "ShaderPixel.h"
#include "Utility.h"
#include "Resources.h"
#include "stb_image.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <algorithm>

namespace Renderer
{
	void Init()
	{
		GLCall(glEnable(GL_DEPTH_TEST));
		stbi_set_flip_vertically_on_load(true);
		//glEnable(GL_MULTISAMPLE); need?

		assert(Resources::Shaders.size() == 0);
		assert(Resources::Textures.size() == 0);
		Resources::Shaders.emplace_back(0); // default shader
		Resources::Textures.emplace_back();// default texture
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

}
