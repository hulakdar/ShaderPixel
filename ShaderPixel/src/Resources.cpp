#include "Resources.h"

namespace Resources {

	std::string				BaseFilepath = "content/";
	std::vector<Mesh>		Meshes;
	std::vector<Texture>	Textures;
	std::vector<Material>	Materials;
	std::vector<Shader>		Shaders;

	std::vector<TextureData>	QueuedTextures;

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

	Material *GetMaterial(MaterialID ID)
	{
		if (ID >= Materials.size())
			return nullptr;
		return &Materials[ID];
	}

	Shader *GetShader(ShaderID ID)
	{
		if (ID >= Shaders.size())
			return nullptr;
		return &Shaders[ID];
	}

	TextureID PromisedTextureID()
	{
		return Textures.size() + QueuedTextures.size();
	}

	void FlushTextureData()
	{
		for (auto& It : QueuedTextures)
		{
			Textures.emplace_back(It);
			free(It.Buffer);
		}
		QueuedTextures.clear();
	}

	void Clear()
	{
		Meshes.clear();
		Textures.clear();
	}
}
