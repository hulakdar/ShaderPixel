#pragma once


#pragma once

// Replace with your own GL header include
#include <glad/glad.h>

#include <vector>
#include <utility>
#include <map>
#include <set>
#include <string>

struct Shader
{
	typedef uint32_t Handle; //index into mHandles;
	Handle mHandle;
};

class ShaderManager
{
private:
	struct Timestamp
	{
		Timestamp(const std::string& vertexPath, const std::string fragmentPath);
		uint64_t vertex;
		uint64_t fragment;
		bool operator < (const Timestamp& Other);
	};
	const GLuint GLOBAL_BLOCK_BINDING_LOCATION = 0;
	std::string mVersion = "#version 410 core\n";
	std::string mPreamble;
	std::map<std::string, Shader::Handle> mHandleMap;
	std::vector<GLuint>			mShaders;
	std::vector<Timestamp>		mTimestamps;
	std::vector<std::string>	mCompileErrors;

	GLuint compileSingleShader(const std::string& filePath, GLenum type);
public:
	ShaderManager() = default;
	~ShaderManager();

	void SetPreamble(const std::string& preambleFilename);
	Shader AddProgram(const std::string& vertexPath, const std::string& fragmentPath);
	void UpdatePrograms();
};
