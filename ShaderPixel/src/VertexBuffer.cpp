#include "VertexBuffer.h"
#include "Utility.h"

#include <glad/glad.h>
#include <algorithm>

unsigned int VertexBuffer::s_CurrentlyBound = 0;

VertexBuffer::VertexBuffer()
{
}

VertexBuffer::VertexBuffer(const void* Data, size_t Size)
{
	Init(Data, Size);
}

void VertexBuffer::Init(const void* Data, size_t Size)
{
	// generating the vertex buffer
	GLCall(glGenBuffers(1, &mRendererID));
	// binding the buffer
	Bind();
	// transferring the buffer to GPU
	GLCall(glBufferData(GL_ARRAY_BUFFER, Size, Data, GL_STATIC_DRAW));
}

VertexBuffer::VertexBuffer(VertexBuffer &&Other)
{
	*this = std::move(Other);
}

VertexBuffer & VertexBuffer::operator=(VertexBuffer &&Other)
{
	std::swap(Other.mRendererID, mRendererID);
	return *this;
}

VertexBuffer::~VertexBuffer()
{
	//if (mRendererID)
		//GLCall(glDeleteBuffers(1, &mRendererID));
}

void VertexBuffer::Bind(void) const
{
	//if (mRendererID == s_CurrentlyBound)
	//	return;
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, mRendererID));
	s_CurrentlyBound = mRendererID;
}

void VertexBuffer::Unbind(void) const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	s_CurrentlyBound = 0;
}
