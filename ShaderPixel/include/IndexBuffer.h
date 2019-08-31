#pragma once
#include <vector>
#include <tiny_obj_loader.h>

class IndexBuffer
{
private:
	static unsigned int s_CurrentlyBound;

	unsigned int mRendererID;
	unsigned int mCount;
public:
	IndexBuffer(const unsigned int *Data = nullptr, unsigned int Count = 0);
	IndexBuffer(const std::vector<unsigned int>& Data);
	IndexBuffer(const std::vector<tinyobj::index_t>& indices, const std::vector<unsigned char>& num_faces);

	IndexBuffer(const IndexBuffer&) = delete;
	IndexBuffer& operator=(const IndexBuffer&) = delete;

	IndexBuffer(IndexBuffer&&);
	IndexBuffer& operator=(IndexBuffer&&);

	~IndexBuffer();

	void Init(const unsigned int *Data, unsigned int Count);

	void Bind(void) const;
	void Unbind(void) const;
	unsigned int GetCount(void) const;
};

