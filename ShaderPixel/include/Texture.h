#pragma once
#include "glm/glm.hpp"

#include <string>
#include <glad/glad.h>

enum TextureUsage : uint16_t
{
	Ambient,
	Diffuse,
	Specular,
	Specular_highlight,
	Bump,
	Displacement,
	Alpha,
	Emissive,
	Noise,
};

typedef size_t	TextureID;

struct TextureData
{
	uint8_t		*Buffer;
	glm::ivec2	Size;
	int			ComponentCount;
	int			Slices = 1;
	GLuint		Type = GL_TEXTURE_2D;
};

struct TextureBinding {
	TextureUsage	usage;
	TextureID		id;
};

class Texture
{
public:
	// special purpose textures
	enum
	{
		Default = 0,
		Cloud = 1,
		Noise = 2,
	};
	
	Texture(TextureData&);
	Texture(const std::string& Filepath = "face.png");
	Texture(Texture&&);
	Texture& operator=(Texture&& Other);
	~Texture();

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture& Other) = delete;

	void Bind(unsigned int Slot = 0) const;
	void Unbind() const;
	void uploadData(uint8_t* LocalBuffer, glm::ivec2 Size, int ComponentCount, GLuint Type, int slices);

private:
	GLuint			mRendererID;
	GLuint			mType = GL_TEXTURE_2D;

	glm::ivec2		mSize{0,0};
	int				mComponentCount = 0;
};

