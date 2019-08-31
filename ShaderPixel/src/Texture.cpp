#include "Texture.h"
#include "stb_image.h"
#include <iostream>
#include <glad/glad.h>
#include <map>

static std::map<std::string, Texture> s_TextureCache;
static unsigned int sCurrentlyBound = 0;

Texture::Texture(const std::string& Filepath) :
	mRendererID(0),
	mFilepath(Filepath)
{
//	auto FindIterator = s_TextureCache.find(Filepath);
//	if (FindIterator != s_TextureCache.end())
//	{
//		*this = FindIterator->second;
//		return;
//	}

	uint8_t			*mLocalBuffer;
	mLocalBuffer = stbi_load(Filepath.c_str(), &mSize.x, &mSize.y, &mComponentCount, 4);
	
	if (!mLocalBuffer)
	{
		__debugbreak();
	}
	glGenTextures(1, &mRendererID);
	Bind();

	glTexParameteri(mType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(mType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(mType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(mType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (mComponentCount == 4)
		glTexImage2D(mType, 0, GL_RGBA8, mSize.x, mSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, mLocalBuffer);
	else
		glTexImage2D(mType, 0, GL_RGB8, mSize.x, mSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, mLocalBuffer);
	Unbind();
	//s_TextureCache.emplace(Filepath, *this);
	free(mLocalBuffer);
	std::cerr << "A new texture loaded: " << Filepath << "\n";
}

Texture::Texture(const Texture &&Other)
{
	memcpy(this, &Other, sizeof(*this));
}

Texture::~Texture()
{
	glDeleteTextures(1, &mRendererID);
}

void Texture::Bind(unsigned int Slot) const
{
	//if (mRendererID == s_CurrentlyBound)
	//	return;
	glActiveTexture(GL_TEXTURE0 + Slot);
	glBindTexture(GL_TEXTURE_2D, mRendererID);
	//s_CurrentlyBound = mRendererID;
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE0, 0);
	//s_CurrentlyBound = 0;
}
