#pragma once
#include <vector>

static const float cubeVertexBufferData[] =
{
	-0.5f, -0.5f, -0.5f, 0.0f,0.0f, 0.0f,0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f, 1.0f,0.0f, 0.0f,0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f, 1.0f,1.0f, 0.0f,0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f, 1.0f,1.0f, 0.0f,0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f, 0.0f,1.0f, 0.0f,0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f, 0.0f,0.0f, 0.0f,0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f, 0.0f,0.0f, 0.0f,0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f, 1.0f,0.0f, 0.0f,0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f, 1.0f,1.0f, 0.0f,0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f, 1.0f,1.0f, 0.0f,0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f, 0.0f,1.0f, 0.0f,0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, 0.0f,0.0f, 0.0f,0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, 1.0f,0.0f, -1.0f,0.0f,0.0f,
	-0.5f,  0.5f, -0.5f, 1.0f,1.0f, -1.0f,0.0f,0.0f,
	-0.5f, -0.5f, -0.5f, 0.0f,1.0f, -1.0f,0.0f,0.0f,
	-0.5f, -0.5f, -0.5f, 0.0f,1.0f, -1.0f,0.0f,0.0f,
	-0.5f, -0.5f,  0.5f, 0.0f,0.0f, -1.0f,0.0f,0.0f,
	-0.5f,  0.5f,  0.5f, 1.0f,0.0f, -1.0f,0.0f,0.0f,

	 0.5f,  0.5f,  0.5f, 1.0f,0.0f, 1.0f,0.0f, 0.0f,
	 0.5f,  0.5f, -0.5f, 1.0f,1.0f, 1.0f,0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f, 0.0f,1.0f, 1.0f,0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f, 0.0f,1.0f, 1.0f,0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f, 0.0f,0.0f, 1.0f,0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f, 1.0f,0.0f, 1.0f,0.0f, 0.0f,

	-0.5f, -0.5f, -0.5f, 0.0f,1.0f, 0.0f,-1.0f,0.0f,
	 0.5f, -0.5f, -0.5f, 1.0f,1.0f, 0.0f,-1.0f,0.0f,
	 0.5f, -0.5f,  0.5f, 1.0f,0.0f, 0.0f,-1.0f,0.0f,
	 0.5f, -0.5f,  0.5f, 1.0f,0.0f, 0.0f,-1.0f,0.0f,
	-0.5f, -0.5f,  0.5f, 0.0f,0.0f, 0.0f,-1.0f,0.0f,
	-0.5f, -0.5f, -0.5f, 0.0f,1.0f, 0.0f,-1.0f,0.0f,

	-0.5f,  0.5f, -0.5f, 0.0f,1.0f, 0.0f,1.0f,0.0f,
	 0.5f,  0.5f, -0.5f, 1.0f,1.0f, 0.0f,1.0f,0.0f,
	 0.5f,  0.5f,  0.5f, 1.0f,0.0f, 0.0f,1.0f,0.0f,
	 0.5f,  0.5f,  0.5f, 1.0f,0.0f, 0.0f,1.0f,0.0f,
	-0.5f,  0.5f,  0.5f, 0.0f,0.0f, 0.0f,1.0f,0.0f,
	-0.5f,  0.5f, -0.5f, 0.0f,1.0f, 0.0f,1.0f,0.0f
};


class VertexBuffer
{
private:
	static unsigned int s_CurrentlyBound;

	unsigned int mRendererID;
public:
	VertexBuffer(const void *Data = cubeVertexBufferData, size_t Size = sizeof(cubeVertexBufferData));

	template<typename T>
	VertexBuffer(const std::vector<T>& Data);

	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer& operator=(const VertexBuffer&) = delete;

	VertexBuffer(VertexBuffer&&);
	VertexBuffer& operator=(VertexBuffer&&);

	~VertexBuffer();

	void Bind(void) const ;
	void Unbind(void) const ;
};

template<typename T>
inline VertexBuffer::VertexBuffer(const std::vector<T>& Data) :
	VertexBuffer(Data.data(), Data.size() * sizeof(T))
{}
