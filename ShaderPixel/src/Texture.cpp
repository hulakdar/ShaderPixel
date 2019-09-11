#include "Texture.h"
#include "Resources.h"
#include "stb_image.h"
#include "Utility.h"

#include <iostream>
#include <glad/glad.h>
#include <map>

static std::map<std::string, Texture> s_TextureCache;
static unsigned int sCurrentlyBound = 0;


Texture::Texture(const std::string& Filepath) :
	mRendererID(0)
{
//	auto FindIterator = s_TextureCache.find(Filepath);
//	if (FindIterator != s_TextureCache.end())
//	{
//		*this = FindIterator->second;
//		return;
//	}

	std::string CompleteFilepath(Resources::BaseFilepath + Filepath);

	uint8_t		*LocalBuffer;
	glm::ivec2	Size;
	int			ComponentCount;
	LocalBuffer = stbi_load(CompleteFilepath.c_str(), &Size.x, &Size.y, &ComponentCount, 0);
	
	if (!LocalBuffer)
	{
		__debugbreak();
	}
	uploadData(LocalBuffer, Size, ComponentCount);
	free(LocalBuffer);
}

Texture::Texture(TextureData& data)
{
	uploadData(data.Buffer, data.Size, data.ComponentCount);
}

void Texture::uploadData(uint8_t *LocalBuffer, glm::ivec2 Size, int ComponentCount)
{
	glGenTextures(1, &mRendererID);
	Bind();
	mComponentCount = ComponentCount;
	mSize = Size;

	glTexParameteri(mType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(mType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(mType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(mType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (mComponentCount == 4)
		glTexImage2D(mType, 0, GL_RGBA, mSize.x, mSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, LocalBuffer);
	else if (mComponentCount == 3)
		glTexImage2D(mType, 0, GL_RGB, mSize.x, mSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, LocalBuffer);
	else if (mComponentCount == 2)
		glTexImage2D(mType, 0, GL_RG, mSize.x, mSize.y, 0, GL_RG, GL_UNSIGNED_BYTE, LocalBuffer);
	else if (mComponentCount == 1)
		glTexImage2D(mType, 0, GL_RED, mSize.x, mSize.y, 0, GL_RED, GL_UNSIGNED_BYTE, LocalBuffer);
	else
		__debugbreak();
	GLCall(glGenerateMipmap(GL_TEXTURE_2D));
}

Texture::Texture(Texture&& Other)
{
	*this = std::move(Other);
}

Texture& Texture::operator=(Texture&& Other)
{
	mComponentCount = Other.mComponentCount;
	mRendererID = Other.mRendererID;
	mSize = Other.mSize;
	mType = Other.mType;
	return *this;
}

Texture::~Texture()
{
	//glDeleteTextures(1, &mRendererID);
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

