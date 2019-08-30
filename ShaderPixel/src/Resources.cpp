#include "Resources.h"

namespace Resources {

	std::vector<Mesh>		Meshes;
	std::vector<Texture>	Textures;

	Mesh	*GetMesh(MeshID ID)
	{
		if (ID >= Meshes.size())
			return nullptr;
		return &Meshes[ID];
	}

	Texture *GetTexture(TextureID ID)
	{
		if (ID >= Textures.size())
			return nullptr;
		return &Textures[ID];
	}

	void Clear()
	{
		Meshes.clear();
		Textures.clear();
	}
}
