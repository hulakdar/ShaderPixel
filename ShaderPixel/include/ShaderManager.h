#pragma once

/*
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
		operator bool ();
	};
	typedef uint32_t Handle; //index into mShaders
	Handle mHandle;

	void Bind();
	void Uniform(const std::string& name, float scalar);
};

class ShaderManager
{
	friend Shader;
private:
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
*/
