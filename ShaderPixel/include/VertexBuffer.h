#pragma once
#include <vector>

class VertexBuffer
{
private:
	static unsigned int s_CurrentlyBound;

	unsigned int mRendererID = 0;
public:
	VertexBuffer();
	VertexBuffer(const void *Data, size_t Size);

	template<typename T>
	VertexBuffer(const std::vector<T>& Data);

	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer& operator=(const VertexBuffer&) = delete;

	VertexBuffer(VertexBuffer&&);
	VertexBuffer& operator=(VertexBuffer&&);

	~VertexBuffer();

	void Init(const void* Data, size_t Size);
	void Bind(void) const ;
	void Unbind(void) const ;
};

template<typename T>
inline VertexBuffer::VertexBuffer(const std::vector<T>& Data) :
	VertexBuffer(Data.data(), Data.size() * sizeof(T))
{}
