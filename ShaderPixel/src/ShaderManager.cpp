#include "ShaderManager.h"
#include "Host.h"

#include <string>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <malloc.h>
#include <assert.h>
#include <imgui.h>

#ifdef _WIN32
#include <sys/stat.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

namespace Utils {

	std::string StringFromFile(const std::string& filename)
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
}

void Shader::Timestamp::update(const std::string& vertexPath, const std::string fragmentPath)
{
	vertex = Utils::GetFileTimestamp(vertexPath.c_str());
	fragment = Utils::GetFileTimestamp(fragmentPath.c_str());
}

bool Shader::Timestamp::operator==(const Timestamp& Other)
{
	return vertex == Other.vertex && fragment == Other.fragment;
}
bool Shader::Timestamp::operator!=(const Timestamp& Other)
{
	return !(*this == Other);
}

ShaderManager::~ShaderManager()
{
	for (auto shader : sShaders)
	{
		glDeleteProgram(shader);
	}
}

Shader ShaderManager::GetShader(const std::string& vertexPath, const std::string& fragmentPath)
{
	const std::string key = vertexPath + ":" + fragmentPath;

	auto it = sHandleMap.find(key);
	if (it != sHandleMap.end())
		return Shader{ it->second };

	Shader result{ (uint32_t)sShaders.size() };
	sTimestamps.emplace_back();
	sCompileErrors.emplace_back("");
	sShaders.emplace_back(0);
	assert(sTimestamps.size() == sShaders.size() && sCompileErrors.size() == sTimestamps.size());
	sHandleMap.emplace(key, result.mHandle);
	return result;
}

void ShaderManager::UpdatePrograms()
{
	for (auto& handle : sHandleMap)
	{
		const std::string &paths = handle.first;
		size_t colon = paths.find(':');

		assert(colon != paths.npos);

		std::string vertexPath = paths.substr(0, colon);
		std::string fragmentPath = paths.substr(colon+1, std::string::npos);
		Shader::Timestamp currentTimestamp;
		currentTimestamp.update(vertexPath, fragmentPath);

		uint32_t index = handle.second;

		auto &timestamp = sTimestamps[index];
		if (timestamp != currentTimestamp)
		{
			timestamp = currentTimestamp;
			std::string& compileError = sCompileErrors[index];
			compileError.clear();
			auto compileSingleShader = [&](const std::string& filePath, GLenum type) -> GLuint
			{
				GLuint result = glCreateShader(type);
				std::string text = mVersion + mPrefix + Utils::StringFromFile(filePath);

				const char* data = text.data();
				glShaderSource(result, 1, &data, NULL);
				glCompileShader(result);

				GLint status;
				glGetShaderiv(result, GL_COMPILE_STATUS, &status);
				if (status == GL_FALSE)
				{
					glGetShaderiv(result, GL_INFO_LOG_LENGTH, &status);
					char* message = (char*)_malloca(status);
					glGetShaderInfoLog(result, status, &status, message);
					if (message)
						compileError += message;
				}
				return result;
			};
			GLuint program = glCreateProgram();

			{   // link program
				GLuint vertexShader = compileSingleShader(vertexPath, GL_VERTEX_SHADER);
				GLuint fragmentShader = compileSingleShader(fragmentPath, GL_FRAGMENT_SHADER);
				assert(vertexShader && fragmentShader);
				glAttachShader(program, vertexShader);
				glAttachShader(program, fragmentShader);
				glLinkProgram(program);
				glValidateProgram(program);
				if (vertexShader) glDeleteShader(vertexShader);
				if (fragmentShader) glDeleteShader(fragmentShader);
			}

			{   // check errors
				GLint linkStatus, valid;
				glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
				glGetProgramiv(program, GL_VALIDATE_STATUS, &valid);

				if (linkStatus == GL_FALSE || valid == GL_FALSE)
				{
					GLint status;
					glGetProgramiv(program, GL_INFO_LOG_LENGTH, &status);
					char *message = (char *)_malloca(status);
					glGetProgramInfoLog(program, status, &status, message);
					if (message)
						compileError += message;
					glDeleteProgram(program);
					continue;
				}
			}
			// this may be useful
			GLuint blockIndex = glGetUniformBlockIndex(program, "global");
			if (blockIndex != GL_INVALID_INDEX)
				glUniformBlockBinding(program, blockIndex, GLOBAL_BLOCK_BINDING_LOCATION);
			GLuint& oldProgram = sShaders[index];
			if (oldProgram)
				glDeleteProgram(oldProgram);
			oldProgram = program;
			break;
		}
	}

	ImGui::Begin("CompileErrors");
	for (auto handle : sHandleMap)
	{
		std::string& errorMessage = sCompileErrors[handle.second];
		if (errorMessage.size())
		{
			ImGui::TextWrapped("%s:\n", handle.first.data());
			ImGui::TextWrapped("%s\n\n", errorMessage.data());
		}
	}
	ImGui::End();
}

void ShaderManager::setPrefix(const std::string& prefixFilename)
{
	mPrefix = Utils::StringFromFile(prefixFilename);
}

std::map<std::string, Shader::Handle>	ShaderManager::sHandleMap;
std::vector<Shader::Timestamp>			ShaderManager::sTimestamps;
std::vector<std::string>				ShaderManager::sCompileErrors;
std::vector<GLuint>						ShaderManager::sShaders;

void Shader::Bind()
{
	glUseProgram(ShaderManager::sShaders[mHandle]);
}
