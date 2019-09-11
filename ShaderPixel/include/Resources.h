#pragma once
#include <vector>
#include "Mesh.h"
#include "Texture.h"

namespace Resources
{
	extern std::string				BaseFilepath;
	extern std::vector<Mesh>		Meshes;
	extern std::vector<Texture>		Textures;
	extern std::vector<Material>	Materials;
	extern std::vector<Shader>		Shaders;

	extern std::vector<TextureData>	QueuedTextures;

	// Never cache these ptr, please
	Mesh		*GetMesh(MeshID);
	Texture		*GetTexture(TextureID);
	Material	*GetMaterial(MaterialID);
	Shader		*GetShader(ShaderID);
	void		FlushTextureData();

	TextureID	PromisedTextureID();
	void Clear();
};
