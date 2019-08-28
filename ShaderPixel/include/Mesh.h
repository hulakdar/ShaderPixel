#include "VertexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "Material.h"

class Mesh
{
private:
    VertexBuffer		m_VertexBuffer;
    VertexBufferLayout	m_VertexBufferLayout;
    VertexArray			m_VertexArray;
    Material            m_Material;
public:
	Mesh(const std::vector<float>& Verteces, const Material& Material);
	Mesh(const std::vector<float>& Verteces, const Shader& Shader);
	Mesh(const std::vector<float>& Verteces);
	Mesh();
};