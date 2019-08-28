#pragma  once

#include "VertexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "Material.h"

class Mesh
{
private:
    VertexBuffer		mVertexBuffer;
    VertexBufferLayout	mVertexBufferLayout;
    VertexArray			mVertexArray;
    Material            mMaterial;
public:
	Mesh(const std::vector<float>& Verteces, const Material& Material);
	Mesh(const std::vector<float>& Verteces, const Shader& Shader);
	Mesh(const std::vector<float>& Verteces);
	Mesh();
};