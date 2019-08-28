#include "VertexBufferLayout.h"
#include "Renderer.h"
#include <glad/glad.h>

unsigned int GetSizeOfType(unsigned int Type)
{
	switch (Type)
	{
	case GL_FLOAT:
		return 4;
	case GL_UNSIGNED_INT:
		return 4;
	case GL_INT:
		return 4;
	case GL_UNSIGNED_BYTE:
		return 1;
	}
	return 0;
}

template <>
void VertexBufferLayout::Push<float>(unsigned int Count)
{
	mElements.push_back({GL_FLOAT, Count, GL_FALSE});
	mStride += GetSizeOfType(GL_FLOAT) * Count;
}

template <>
void VertexBufferLayout::Push<unsigned int>(unsigned int Count)
{
	mElements.push_back({GL_UNSIGNED_INT, Count, GL_FALSE});
	mStride += GetSizeOfType(GL_UNSIGNED_INT) * Count;
}

template <>
void VertexBufferLayout::Push<int>(unsigned int Count)
{
	mElements.push_back({GL_INT, Count, GL_FALSE});
	mStride += GetSizeOfType(GL_INT) * Count;
}

template <>
void VertexBufferLayout::Push<unsigned char>(unsigned int Count)
{
	mElements.push_back({GL_UNSIGNED_BYTE, Count, GL_TRUE});
	mStride += GetSizeOfType(GL_UNSIGNED_BYTE) * Count;
}

VertexBufferLayout::VertexBufferLayout()
	: mStride(0)
{
}


VertexBufferLayout::~VertexBufferLayout()
{
}

const std::vector<VertexBufferElement>& VertexBufferLayout::GetElements() const
{
	return mElements;
}

unsigned int VertexBufferLayout::GetStride() const
{
	return mStride;
}
