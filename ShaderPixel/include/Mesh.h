#pragma  once

#include "VertexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "Material.h"
#include "AABB.h"

struct Vertex
{
	glm::vec3	Position;
	glm::vec3	Normal;
	glm::vec2	UV;
};

typedef size_t MeshID;
struct Mesh
{
    VertexBuffer		vertexBuffer;
    //IndexBuffer			mIndexBuffer;
    VertexArray			vertexArray;
    MaterialID          materialID;
	unsigned int		count = 0; // if zero then it's indexed so use IndexBuffer
	AABB				bounds;
};

struct Model;

void	LoadMesh(const tinyobj::mesh_t& mesh,
				const tinyobj::attrib_t& attributes,
				Model& model, size_t MaterialIdOffset);

