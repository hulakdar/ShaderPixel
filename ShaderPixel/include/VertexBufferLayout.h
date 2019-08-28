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
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
public:
	VertexBufferLayout();
	~VertexBufferLayout();

	//template<typename T>
	//void Push(unsigned int Count)
	//{
	//	(void)Count;
	//	//SDL_assert(false);
	//	std::cerr << "Trying to push invalid type to VertexBufferLayout\n";
	//	exit(1);
	//}

	template<typename T>
	void Push(unsigned int Count);

	const std::vector<VertexBufferElement>& GetElements() const ;
	unsigned int GetStride() const ;
};

