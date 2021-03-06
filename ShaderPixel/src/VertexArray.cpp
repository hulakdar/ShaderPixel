#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Utility.h"
#include <glad/glad.h>

unsigned int VertexArray::s_CurrentlyBound = 0;

VertexArray::VertexArray() { }

VertexArray::VertexArray(const VertexBuffer & Vb, const VertexBufferLayout & Vbl) :
	VertexArray()
{
	AddBuffer(Vb, Vbl);
}

VertexArray::~VertexArray()
{
	//GLCall(glDeleteVertexArrays(1, &mRendererID));
}

VertexArray::VertexArray(VertexArray&& Other)
{
	*this = std::move(Other);
}

VertexArray& VertexArray::operator=(VertexArray&& Other)
{
	std::swap(mRendererID, Other.mRendererID);
	return *this;
}

void VertexArray::AddBuffer(const VertexBuffer& Vb, const VertexBufferLayout& Layout, unsigned int attributeOffset)
{
	if (mRendererID == 0)
		GLCall(glGenVertexArrays(1, &mRendererID));
	Vb.Bind();
	Bind();

	const auto& Elements = Layout.GetElements();
	unsigned long long Offset = 0;
	for (unsigned int i = 0 ; i < Elements.size(); ++i)
	{
		const auto& Element = Elements[i];
		// binding the array
		GLCall(glEnableVertexAttribArray(i + attributeOffset));
		// setting up vertex position attribute
		GLCall(glVertexAttribPointer(i + attributeOffset, Element.Count, Element.Type, Element.Normalized, Layout.GetStride(), (const void *)Offset));

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
