#pragma once

#include <glm/glm.hpp>
#include <vector>

struct AABB
{
	glm::vec3 Min = glm::vec3( INFINITY);
	glm::vec3 Max = glm::vec3(-INFINITY);

	void extend(glm::vec3 position);
};

struct Vertex;

AABB GetMeshAABB(std::vector<Vertex>& vertexData);

