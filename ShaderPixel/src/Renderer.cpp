#include "Renderer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "ShaderPixel.h"
#include <iostream>
#include "Wrapper.h"

#include <glad/glad.h>

namespace Renderer
{
	void Init()
	{
		GLCall(glEnable(GL_DEPTH_TEST));

		//GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	}

	void ApplyMaterial(Material& material, Shader shader)
	{
		for (auto& It : material.mUniforms)
		{
			shader.SetUniform(It);
		}
	}

	void Clear()
	{
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}

	void Draw(Scene& scene, Shader shader, glm::mat4 viewProjection)
	{
		for (auto& model : scene.mModels)
		{
			glm::mat4 MVP = viewProjection * model.mModelSpace;
			for (auto& mesh : model.mMeshes)
			{
				ApplyMaterial(mesh.mMaterial, shader);
				shader.SetUniform("uMVP", MVP);
				Draw(mesh);
			}
		}
	}

	void Draw(Mesh& mesh)
	{
		if (mesh.mCount)
			Draw(mesh.mVertexArray, mesh.mCount);
		else
			Draw(mesh.mVertexArray, mesh.mIndexBuffer);
	}

	void Draw(const VertexArray& va, const IndexBuffer& ib)
	{
		va.Bind();
		ib.Bind();
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
