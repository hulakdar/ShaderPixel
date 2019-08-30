#pragma once
#include <vector>
#include "Mesh.h"
#include "Texture.h"

namespace Resources
{
	extern std::vector<Mesh>	Meshes;
	extern std::vector<Texture>	Textures;

	// Never cache these ptr, please
	Mesh		*GetMesh(MeshID);
	Texture		*GetTexture(TextureID);
	void Clear();
};
