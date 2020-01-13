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

	return;


	







	if (position.x < Min.x)
		Min.x = position.x;
	if (position.y < Min.y)
		Min.y = position.y;
	if (position.z < Min.z)
		Min.z = position.z;
	if (position.x > Max.x)
		Max.x = position.x;
	if (position.y > Max.y)
		Max.y = position.y;
	if (position.z > Max.z)
		Max.z = position.z;
}
