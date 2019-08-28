#pragma once

class VertexArray
{
private:
	static unsigned int s_CurrentlyBound;

	unsigned int m_RendererID;
public:
	VertexArray();
	VertexArray(const class VertexBuffer& Vb, const class VertexBufferLayout& Vbl);
	~VertexArray();

	void AddBuffer(const class VertexBuffer& Vb, const class VertexBufferLayout& Vbl);
	void Bind() const ;
	void Unbind() const ;
};

