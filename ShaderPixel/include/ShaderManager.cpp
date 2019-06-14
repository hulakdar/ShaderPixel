#include "ShaderManager.h"

#ifdef _WIN32
#include <sys/stat.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include <string>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <malloc.h>

static
uint64_t GetFileTimestamp(const char* filename)
{
	uint64_t timestamp = 0;

#ifdef _WIN32
	struct __stat64 stFileInfo;
	if (_stat64(filename, &stFileInfo) == 0)
	{
		timestamp = stFileInfo.st_mtime;
	}
#else
	struct stat fileStat;

	if (stat(filename, &fileStat) == -1)
	{
		perror(filename);
		return 0;
	}

#ifdef __APPLE__
	timestamp = fileStat.st_mtimespec.tv_sec;
#else
	timestamp = fileStat.st_mtime;
#endif
#endif

	return timestamp;
}

static
std::string ShaderStringFromFile(const std::string& filename)
{
	std::ifstream fs(filename);
	if (fs.bad())
	{
		return "";
	}

	std::string s(
		std::istreambuf_iterator<char>{fs},
		std::istreambuf_iterator<char>{});

	return s;
}


ShaderManager::Timestamp::Timestamp(const std::string& vertexPath, const std::string fragmentPath)
	: vertex (GetFileTimestamp(vertexPath.c_str()))
	, fragment (GetFileTimestamp(fragmentPath.c_str()))
{
}

bool ShaderManager::Timestamp::operator < (const Timestamp& Other)
{
	return vertex < Other.vertex || fragment < Other.fragment;
}

ShaderManager::~ShaderManager()
{
	for (auto& shader : mShaders)
	{
		glDeleteProgram(shader);
	}
}

GLuint ShaderManager::compileSingleShader(const std::string& filePath, GLenum type)
{
	GLuint result = glCreateShader(type);
	std::string text = mVersion + mPreamble + ShaderStringFromFile(filePath);

	const char* data = text.data();
	glShaderSource(result, 1, &data, NULL);
	glCompileShader(result);

	GLint status;
	glGetShaderiv(result, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(result, GL_INFO_LOG_LENGTH, &status);
		char *message = (char *)_alloca(status);
		glGetShaderInfoLog(result, status, &status, message);
		mCompileErrors += message;
		return 0;
	}
	return result;
}

Shader ShaderManager::AddProgram(const std::string& vertexPath, const std::string& fragmentPath)
{
	const std::string key = vertexPath + ":" + fragmentPath;

	auto it = mHandleMap.find(key);
	if (it != mHandleMap.end())
		return Shader{ it->second };

	Shader result{ mShaders.size() };

	GLuint vertexShader = compileSingleShader(vertexPath, GL_VERTEX_SHADER);
	GLuint fragmentShader = compileSingleShader(fragmentPath, GL_FRAGMENT_SHADER);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	glValidateProgram(program);
	if (vertexShader) glDeleteShader(vertexShader);
	if (fragmentShader) glDeleteShader(fragmentShader);
	mHandleMap.emplace(key, result);
	mShaders.push_back(program);

	GLint linkStatus, valid;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	glGetProgramiv(program, GL_VALIDATE_STATUS, &valid);

	if (linkStatus == GL_FALSE || valid == GL_FALSE)
	{
		GLint status;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &status);
		char *message = (char *)_alloca(status);
		glGetProgramInfoLog(program, status, &status, message);
		mCompileErrors += message;
		glDeleteProgram(program);
		return result;
	}

	// this may be useful
	GLuint blockIndex = glGetUniformBlockIndex(program, "global");
	if (blockIndex != GL_INVALID_INDEX)
		glUniformBlockBinding(program, blockIndex, GLOBAL_BLOCK_BINDING_LOCATION);
	return result;
}

void ShaderManager::UpdatePrograms()
{
	// find all shaders with updated timestamps
	for (auto handle : mHandleMap)
	{
		const std::string &paths = handle.first;
		uint32_t colon = paths.find(':');

		std::string vertex = paths.substr(0, colon);
		std::string fragment = paths.substr(colon+1, std::string::npos);
		Timestamp currentTimestamp(vertex, fragment);

		uint32_t index = handle.second;

		Timestamp &timestamp = mTimestamps[index];
		GLuint& shader = mShaders[index];
		if (currentTimestamp < timestamp)
		{
			// TODO: mark errors as stale
			// TODO: recompile shader
			timestamp = currentTimestamp;
		}
	}
}

void ShaderManager::SetPreamble(const std::string& preambleFilename)
{
	mPreamble = ShaderStringFromFile(preambleFilename);
}

