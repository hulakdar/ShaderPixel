#include "Renderer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include <iostream>
#include "Wrapper.h"

namespace Renderer
{
	void Init()
	{
		GLCall(glEnable(GL_DEPTH_TEST));

		//GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	}

	void Clear()
	{
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}

	void Draw(const VertexArray& Va, const Shader& Program, const IndexBuffer& Ib)
	{
		Program.Bind();
		Va.Bind();
		Ib.Bind();
		GLCall(glDrawElements(GL_TRIANGLES, Ib.GetCount(), GL_UNSIGNED_INT, 0));
	}

	void Draw(const VertexArray& Va, const Shader& Program, unsigned int Count)
	{
		Program.Bind();
		Va.Bind();
		GLCall(glDrawArrays(GL_TRIANGLES, 0, Count));
	}

}
