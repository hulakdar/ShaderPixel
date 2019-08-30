#pragma once
#include <string>
#include "glm/glm.hpp"

typedef uint32_t TextureID;
class Texture
{
public:
	unsigned int	mRendererID;
	Texture(const std::string& Filepath = "../content/face.png");
	Texture(const Texture& Other);
	Texture& operator=(const Texture& Other);
	~Texture();

	void Bind(unsigned int Slot = 0) const;
	void Unbind() const;

	glm::ivec2		mSize{0,0};
	int				mComponentCount = 0;
	std::string		mFilepath;
};

