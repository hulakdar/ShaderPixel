#pragma once

#include <glad/glad.h>
#include <vector>
#include <utility>
#include <map>
#include <set>
#include <string>
#include "Utils.h"

struct Shader
{
	struct Timestamp {
		::Timestamp vert;
		::Timestamp frag;
		void update(const std::string&, const std::string&);
		bool operator == (const Timestamp& Other);
		bool operator != (const Timestamp& Other);
	};
	typedef uint32_t Handle; //index into mHandles;
	Handle mHandle;

	void Bind();
};

class ShaderManager
{
	friend Shader;
private:
	const GLuint GLOBAL_BLOCK_BINDING_LOCATION = 0;
	std::string mVersion = "#version 410 core\n";
	std::string mPrefix;
	std::map<std::string, Shader::Handle>	mHandleMap;
	std::vector<Shader::Timestamp>			mTimestamps;
	std::vector<GLuint>						mShaders;
	std::vector<std::string>				mCompileErrors;

public:
	ShaderManager() = default;
	~ShaderManager();

	void	setPrefix(const std::string& preambleFilename);
	Shader	getShader(const std::string& vertexPath, const std::string& fragmentPath);
	void	updatePrograms();
};
