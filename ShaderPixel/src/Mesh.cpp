#include "Mesh.h"
#include "Model.h"

#include <assert.h>
#include "Resources.h"

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

void	LoadMesh(	const tinyobj::mesh_t& mesh,
					const tinyobj::attrib_t& attributes,
					Model& model)
{
	std::map<int, std::vector<Vertex>> submeshes;

	size_t scrubber = 0;
	for (size_t i = 0; i < mesh.num_face_vertices.size(); i++)
	{
		unsigned char	num = mesh.num_face_vertices[i];
		MaterialID		matID = mesh.material_ids[i];

		std::vector<Vertex>&		vertexData = submeshes[matID];

		vertexData.push_back(MakeVertex(mesh, attributes, scrubber + 0));
		vertexData.push_back(MakeVertex(mesh, attributes, scrubber + 1));
		vertexData.push_back(MakeVertex(mesh, attributes, scrubber + 2));

		for (unsigned char count = num; count > 3; --count)
		{
			vertexData.push_back(MakeVertex(mesh, attributes, scrubber + count - 4));
			vertexData.push_back(MakeVertex(mesh, attributes, scrubber + count - 2));
			vertexData.push_back(MakeVertex(mesh, attributes, scrubber + count - 1));
		}
		scrubber += num;
	}

	for (auto It : submeshes)
	{
		MeshID NewMeshID = (MeshID)Resources::Meshes.size();
		model.mMeshes.push_back(NewMeshID);
		MaterialID					matID = It.first;
		std::vector<Vertex>&   vertexData = It.second;
		Resources::Meshes.push_back({
			VertexBuffer(vertexData),
			VertexArray(),
			matID,
			(unsigned int)vertexData.size()
		});
		Mesh* mesh = Resources::GetMesh(NewMeshID);

		VertexBufferLayout	vbl;
		vbl.Push<float>(3);
		vbl.Push<float>(3);
		vbl.Push<float>(2);
		mesh->vertexArray.AddBuffer(mesh->vertexBuffer, vbl);
	}
}
