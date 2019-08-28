#pragma once
#include <string>
#include "glm/glm.hpp"

class Texture
{
private:

	unsigned int	mRendererID;
	uint8_t			*mLocalBuffer;
public:
	Texture(const std::string& Filepath = "../content/face.png");
	Texture(const Texture& Other);
	Texture& operator=(const Texture& Other);
	~Texture();

	void Bind(unsigned int Slot = 0) const;
	void Unbind() const;

	glm::ivec2		mSize;
	int				mComponentCount;
	std::string		mFilepath;
};

