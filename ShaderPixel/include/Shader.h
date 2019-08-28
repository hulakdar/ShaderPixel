#pragma once

#include <string>
#include <glm/glm.hpp>
#include <map>
#include "Uniform.h"

class Shader
{
private:

	unsigned int mRendererID;
	std::map<std::string, int> LocationCache;
public:
	Shader(const std::string& VertexPath = "../content/shaders/vertDefault.shader",
		const std::string& FragmentPath = "../content/shaders/fragDefault.shader");
	~Shader();

	void Bind() const ;
	void Unbind() const ;

	void SetUniform(const Uniform& Uniform);
	void SetUniform(const std::string& Name, int Value);
	void SetUniform(const std::string& Name, unsigned int value);
	void SetUniform(const std::string& Name, float Value);
	void SetUniform(const std::string& Name, float v0, float v1);
	void SetUniform(const std::string& Name, float v0, float v1, float v2);
	void SetUniform(const std::string& Name, float v0, float v1, float v2, float v3);
	void SetUniform(const std::string& Name, const glm::mat3& value);
	void SetUniform(const std::string& Name, const glm::vec2& value);
	void SetUniform(const std::string& Name, const glm::vec3& value);
	void SetUniform(const std::string& Name, const glm::vec4& value);
	void SetUniform(const std::string& Name, const glm::ivec2& value);
	void SetUniform(const std::string& Name, const glm::ivec3& value);
	void SetUniform(const std::string& Name, const glm::ivec4& value);
	void SetUniform(const std::string& Name, const glm::mat4& value);
private:
	int GetUniformLocation(const std::string& Name);
	static int GetShaderLocation(unsigned int Type, const std::string& Name);
};
