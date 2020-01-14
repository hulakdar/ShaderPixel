#include "AABB.h"
#include <Mesh.h>

AABB GetMeshAABB(std::vector<Vertex>& vertexData)
{
	AABB bounds;
	for (auto& vert : vertexData)
		bounds.extend(vert.Position);

	return bounds;
}

void AABB::extend(glm::vec3 position)
{
	Max = glm::max(position, Max);
	Min = glm::min(position, Min);
}
