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
		BREAK();
	}
	uploadData(LocalBuffer, Size, ComponentCount, GL_TEXTURE_2D, 1);
	free(LocalBuffer);
}

Texture::Texture(TextureData& data)
{
	uploadData(data.Buffer, data.Size, data.ComponentCount, data.Type, data.Slices);
}

void Texture::uploadData(uint8_t *LocalBuffer, glm::ivec2 Size, int ComponentCount, GLuint type, int slices)
{
	glGenTextures(1, &mRendererID);
	Bind();
	mComponentCount = ComponentCount;
	mSize = Size;
	mType = type;

	glTexParameteri(mType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (type == GL_TEXTURE_3D)
	{
		glTexParameteri(mType, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(mType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(mType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(mType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(mType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(mType, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(mType, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	GLenum formatsByComponentCount[] = {0, GL_RED, GL_RG, GL_RGB, GL_RGBA};
	GLenum format = formatsByComponentCount[mComponentCount];
	if (type == GL_TEXTURE_2D)
		glTexImage2D(GL_TEXTURE_2D, 0, format, Size.x, Size.y, 0, format, GL_UNSIGNED_BYTE, LocalBuffer);
	else if (type == GL_TEXTURE_3D)
	{
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, Size.x / slices, Size.y / slices, slices * slices, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, Size.x);
		for (int i = 0; i < slices; i++)
		{
			glm::u8* Row = ((glm::u8*)LocalBuffer) + i * Size.x * (Size.y / slices);
			for (int j = 0; j < slices; j++)
			{
				glm::u8* Col = Row + (j * Size.x / slices);
				GLCall(glTexSubImage3D(GL_TEXTURE_3D, 0,
					0, 0, i * slices + j,
					Size.x / slices, Size.y / slices, 1,
					GL_RED, GL_UNSIGNED_BYTE, (void*)Col));
			}
		}
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	}
	GLCall(glGenerateMipmap(mType));
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
	glBindTexture(mType, mRendererID);
	//s_CurrentlyBound = mRendererID;
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE0, 0);
	//s_CurrentlyBound = 0;
}

