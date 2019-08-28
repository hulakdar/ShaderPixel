#include "Texture.h"
#include "Renderer.h"
#include <iostream>
#include "stb_image.h"

std::unordered_map<std::string, Texture> Texture::s_TextureCache;
unsigned int Texture::s_CurrentlyBound = 0;

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

	mLocalBuffer = stbi_load(Filepath.c_str(), &mSize.x, &mSize.y, &mComponentCount, 4);
	if (!mLocalBuffer)
	{
		__debugbreak();
	}
	glGenTextures(1, &mRendererID);
	Bind();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mSize.x, mSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, mLocalBuffer);
	Unbind();
	//s_TextureCache.emplace(Filepath, *this);
	std::cerr << "A new texture loaded: " << Filepath << "\n";
}

Texture::Texture(const Texture & Other) :
	mRendererID(0),
	mLocalBuffer(nullptr)
{
	*this = Other;
}

Texture& Texture::operator=(const Texture & Other)
{
	mRendererID = Other.mRendererID;
	mFilepath = Other.mFilepath;
	return *this;
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