#pragma once
#include <string>
#include <unordered_map>
#include "glm/glm.hpp"

class Texture
{
private:
	static unsigned int s_CurrentlyBound;
	static std::unordered_map<std::string, Texture> s_TextureCache;

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

