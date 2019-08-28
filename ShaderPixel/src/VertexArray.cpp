#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Wrapper.h"
#include <glad/glad.h>

unsigned int VertexArray::s_CurrentlyBound = 0;

VertexArray::VertexArray()
{
	// generating the vertex buffer
	GLCall(glGenVertexArrays(1, &m_RendererID));
	//Bind();
}

VertexArray::VertexArray(const VertexBuffer & Vb, const VertexBufferLayout & Vbl) :
	VertexArray()
{
	AddBuffer(Vb, Vbl);
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID));
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
	//if (m_RendererID == s_CurrentlyBound)
	//	return;
	GLCall(glBindVertexArray(m_RendererID));
	s_CurrentlyBound = m_RendererID;
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
	s_CurrentlyBound = 0;
} 
