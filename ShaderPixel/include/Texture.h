#pragma once
#include "glm/glm.hpp"

#include <string>
#include <glad/glad.h>

enum TextureUsage : uint16_t
{
	AMBIENT,
	DIFFUSE,
	SPECULAR,
	SPECULAR_HIGHLIGHT,
	BUMP,
	DISPLACEMENT,
	ALPHA,
	REFLECTION,
	EMISSIVE,
};

typedef uint16_t	TextureID;

struct TextureBinding {
	TextureUsage	usage;
	TextureID		id;
};

class Texture
{
public:
	GLuint			mRendererID;
	GLuint			mType = GL_TEXTURE_2D;
	Texture(const std::string& Filepath = "../content/face.png");
	Texture(const Texture&&);
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture& Other) = delete;
	~Texture();

	void Bind(unsigned int Slot = 0) const;
	void Unbind() const;

	glm::ivec2		mSize{0,0};
	int				mComponentCount = 0;
	std::string		mFilepath;
};

