#pragma once

#include <vector>
#include "Mesh.h"

struct Model
{
	glm::mat4			mModelSpace = glm::mat4(1.f);
	std::vector<MeshID>	mMeshes;
	std::string			mName; // for debug. we don't really need this
};