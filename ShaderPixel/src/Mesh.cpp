#include "Mesh.h"
#include <assert.h>

Vertex	MakeVertex(const tinyobj::mesh_t& mesh, const tinyobj::attrib_t& attributes, size_t scrubber)
{
	size_t vIndex = mesh.indices[scrubber].vertex_index;
	size_t uIndex = mesh.indices[scrubber].texcoord_index;
	size_t nIndex = mesh.indices[scrubber].normal_index;

	Vertex tmp;
	tmp.Position = *((glm::vec3*) & attributes.vertices[vIndex * 3]);
	tmp.UV = *((glm::vec2*) & attributes.texcoords[uIndex * 2]);
	tmp.Normal = *((glm::vec3*) & attributes.normals[nIndex * 3]);
	return tmp;
}

Mesh	MakeMesh(	const tinyobj::mesh_t& mesh,
					const tinyobj::attrib_t& attributes,
					const std::vector<tinyobj::material_t> materials)
{
	std::vector<Vertex>			vertexData;

	size_t scrubber = 0;
	for (unsigned char num : mesh.num_face_vertices)
	{
		unsigned char count = num;

		vertexData.push_back(MakeVertex(mesh, attributes, scrubber + 0));
		vertexData.push_back(MakeVertex(mesh, attributes, scrubber + 1));
		vertexData.push_back(MakeVertex(mesh, attributes, scrubber + 2));

		while (count > 3)
		{
			vertexData.push_back(MakeVertex(mesh, attributes, scrubber + count - 4));
			vertexData.push_back(MakeVertex(mesh, attributes, scrubber + count - 2));
			vertexData.push_back(MakeVertex(mesh, attributes, scrubber + count - 1));
			count--;
		}
		scrubber += num;
	}
	return Mesh {
		VertexBuffer(vertexData),
		VertexArray(),
		Material(),
		(unsigned int)vertexData.size()
	};
}
