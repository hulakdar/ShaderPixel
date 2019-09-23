/*
#include "ShaderPixel.h"
#include "Host.h"

#include <cstdio>
#include <algorithm>
#include <malloc.h>
#include <assert.h>
#include <imgui.h>


ShaderManager::~ShaderManager()
{
	for (auto shader : mShaders)
	{
		glDeleteProgram(shader);
	}
}

Shader ShaderManager::getShader(const std::string& vertexPath, const std::string& fragmentPath)
{
	const std::string key = vertexPath + ":" + fragmentPath;

	auto it = mHandleMap.find(key);
	if (it != mHandleMap.end())
		return Shader{ it->second };

	Shader result{ (uint32_t)mShaders.size() };
	mTimestamps.emplace_back();
	mCompileErrors.emplace_back("");
	mShaders.emplace_back(0);
	assert(mTimestamps.size() == mShaders.size() && mCompileErrors.size() == mTimestamps.size());
	mHandleMap.emplace(key, result.mHandle);
	return result;
}

void ShaderManager::updatePrograms()
{
	for (auto& handle : mHandleMap)
	{
		const std::string &paths = handle.first;
		size_t colon = paths.find(':');

		assert(colon != paths.npos);

		std::string vertexPath = paths.substr(0, colon);
		std::string fragmentPath = paths.substr(colon+1, std::string::npos);
		Shader::Timestamp currentTimestamp;
		currentTimestamp.update(vertexPath, fragmentPath);

		uint32_t index = handle.second;

		auto &timestamp = mTimestamps[index];
		if (timestamp != currentTimestamp)
		{
			timestamp = currentTimestamp;

			std::string& compileError = mCompileErrors[index];
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
			// this might be useful
			GLuint blockIndex = glGetUniformBlockIndex(program, "global");
			if (blockIndex != GL_INVALID_INDEX)
				glUniformBlockBinding(program, blockIndex, GLOBAL_BLOCK_BINDING_LOCATION);
			GLuint& oldProgram = mShaders[index];
			if (oldProgram)
				glDeleteProgram(oldProgram);
			oldProgram = program;
			break;
		}
	}

	ImGui::Begin("CompileErrors");
	for (auto handle : mHandleMap)
	{
		std::string& errorMessage = mCompileErrors[handle.second];
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

void Shader::Bind()
{
	auto &host = getHost();
	glUseProgram(host.mMemory->shaderManager.mShaders[mHandle]);
}

void Shader::Uniform(const std::string& name, float scalar)
{
	auto &host = getHost();
	glUseProgram(host.mMemory->shaderManager.mShaders[mHandle]);
	GLint location = glGetUniformLocation(host.mMemory->shaderManager.mShaders[mHandle], name.c_str());
	if (location >= 0)
		glUniform1f(location, scalar);
}

bool Shader::Timestamp::operator!=(const Timestamp& Other)
{
	return !(*this == Other);
}

bool Shader::Timestamp::operator==(const Timestamp& Other)
{
	return vert == Other.vert && frag == Other.frag;
}

Shader::Timestamp::operator bool ()
{
	return (frag && vert);
}

void Shader::Timestamp::update(const std::string& vertPath, const std::string& fragPath)
{
	vert.update(vertPath);
	frag.update(fragPath);
}
*/
