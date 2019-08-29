#pragma once

#include <vector>
#include "Mesh.h"

struct Model
{
	glm::mat4			mModelSpace;
	std::vector<Mesh>	mMeshes;
};