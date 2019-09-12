#pragma once

class VertexArray
{
private:
	static unsigned int s_CurrentlyBound;

	unsigned int mRendererID = 0;
public:
	VertexArray();
	VertexArray(const class VertexBuffer& Vb, const class VertexBufferLayout& Vbl);
	~VertexArray();

	VertexArray(VertexArray&&);
	VertexArray& operator=(VertexArray&&);

	VertexArray(const VertexArray&) = delete;
	VertexArray& operator=(const VertexArray&) = delete;

	void AddBuffer(const class VertexBuffer& Vb, const class VertexBufferLayout& Vbl, unsigned int attributeOffset = 0);
	void Bind() const ;
	void Unbind() const ;
};

