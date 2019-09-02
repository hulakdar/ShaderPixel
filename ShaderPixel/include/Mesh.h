#pragma  once

#include "VertexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "Material.h"

struct Vertex
{
	glm::vec3	Position;
	glm::vec3	Normal;
	glm::vec2	UV;
};

typedef uint32_t MeshID;
struct Mesh
{
    VertexBuffer		mVertexBuffer;
    //IndexBuffer			mIndexBuffer;
    VertexArray			mVertexArray;
    Material            mMaterial;
	unsigned int		mCount = 0; // if zero then it's indexed so use IndexBuffer
};

Mesh MakeMesh(	const tinyobj::mesh_t& mesh,
				const tinyobj::attrib_t& attributes,
				const std::vector<tinyobj::material_t> materials);