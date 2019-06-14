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

	void Bind();
};

class ShaderManager
{
	friend Shader;
private:
	struct Timestamp
	{
		void update(const std::string& vertexPath, const std::string fragmentPath);
		uint64_t vertex = 0;
		uint64_t fragment = 0;
		bool operator == (const Timestamp& Other);
		bool operator != (const Timestamp& Other);
	};
	const GLuint GLOBAL_BLOCK_BINDING_LOCATION = 0;
	std::string mVersion = "#version 410 core\n";
	std::string mPreamble;
	static std::map<std::string, Shader::Handle> sHandleMap;
	static std::vector<GLuint>			sShaders;
	static std::vector<Timestamp>		sTimestamps;
	static std::vector<std::string>		sCompileErrors;

public:
	ShaderManager() = default;
	~ShaderManager();

	void SetPreamble(const std::string& preambleFilename);
	static Shader AddProgram(const std::string& vertexPath, const std::string& fragmentPath);
	void UpdatePrograms();
};
