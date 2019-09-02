#include "IndexBuffer.h"
#include "Renderer.h"
#include "Wrapper.h"

#include <glad/glad.h>
#include <algorithm>

unsigned int IndexBuffer::s_CurrentlyBound = 0;

IndexBuffer::IndexBuffer(const unsigned int *Data, unsigned int Count)
{
	Init(Data, Count);
}

IndexBuffer::IndexBuffer(const std::vector<unsigned int>& Data) : IndexBuffer(Data.data(), static_cast<unsigned int>(Data.size())) {} 

void IndexBuffer::Init(const unsigned int *Data, unsigned int Count)
{
	mCount = Count;
	// generationg the vertex buffer
	GLCall(glGenBuffers(1, &mRendererID));
	// binding the buffer
	Bind();
	// transfering the buffer to gpu
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, Count * sizeof(unsigned int), Data, GL_STATIC_DRAW));
}

IndexBuffer::IndexBuffer(IndexBuffer &&Other)
{
	*this = std::move(Other);
}

IndexBuffer &IndexBuffer::operator=(IndexBuffer &&Other)
{
	std::swap(mRendererID, Other.mRendererID);
	std::swap(mCount, Other.mCount);
	return *this;
}

IndexBuffer::~IndexBuffer()
{
	GLCall(glDeleteBuffers(1, &mRendererID));
}

void IndexBuffer::Bind(void) const
{
	//if (mRendererID == s_CurrentlyBound)
	//	return;
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRendererID));
	s_CurrentlyBound = mRendererID;
}

void IndexBuffer::Unbind(void) const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	s_CurrentlyBound = 0;
}

unsigned int IndexBuffer::GetCount(void) const
{
	return mCount;
}
