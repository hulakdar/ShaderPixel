#pragma once
#include <vector>

class IndexBuffer
{
private:
	static unsigned int s_CurrentlyBound;

	unsigned int mRendererID;
	unsigned int mCount;
public:
	IndexBuffer(const unsigned int *Data, unsigned int Count);
	IndexBuffer(const std::vector<unsigned int>& Data);
	~IndexBuffer();

	void Bind(void) const ;
	void Unbind(void) const ;
	unsigned int GetCount(void) const ;
};

