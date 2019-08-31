#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Wrapper.h"
#include <glad/glad.h>

unsigned int VertexArray::s_CurrentlyBound = 0;

#pragma optimization("", off)

VertexArray::VertexArray()
{
	// generating the vertex buffer
	GLCall(glGenVertexArrays(1, &mRendererID));
	//Bind();
}

VertexArray::VertexArray(const VertexBuffer & Vb, const VertexBufferLayout & Vbl) :
	VertexArray()
{
	AddBuffer(Vb, Vbl);
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &mRendererID));
}

void VertexArray::AddBuffer(const VertexBuffer& Vb, const VertexBufferLayout& Layout)
{
	Vb.Bind();
	Bind();

	const auto& Elements = Layout.GetElements();
	unsigned long long Offset = 0;
	for (unsigned int i = 0 ; i < Elements.size(); ++i)
	{
		const auto& Element = Elements[i];
		// binding the array
		GLCall(glEnableVertexAttribArray(i));
		// setting up vertex position attribute
		GLCall(glVertexAttribPointer(i, Element.Count, Element.Type, Element.Normalized, Layout.GetStride(), (const void *)Offset));

		Offset += Element.Count * GetSizeOfType(Element.Type);
	}
}

void VertexArray::Bind() const
{
	//if (mRendererID == s_CurrentlyBound)
	//	return;
	GLCall(glBindVertexArray(mRendererID));
	s_CurrentlyBound = mRendererID;
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
	s_CurrentlyBound = 0;
} 
