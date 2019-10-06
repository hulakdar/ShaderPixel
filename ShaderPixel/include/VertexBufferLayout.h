#pragma once
#include <vector>
#include <iostream>

unsigned int GetSizeOfType(unsigned int Type);

struct VertexBufferElement
{
	unsigned int Type;
	unsigned int Count;
	unsigned int Normalized;
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> mElements;
	unsigned int mStride;
public:
	VertexBufferLayout();

	template<typename T>
	void Push(unsigned int Count);

	const std::vector<VertexBufferElement>& GetElements() const ;
	unsigned int GetStride() const ;
};

