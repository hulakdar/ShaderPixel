#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

class Shader
{
private:
	static std::unordered_map<std::string, int> s_ShaderCache;
	static unsigned int s_CurrentlyBound;

	unsigned int m_RendererID;
	std::unordered_map<std::string, int> LocationCache;
public:
	Shader(const std::string& VertexPath = "../content/shaders/vertDefault.shaders",
		const std::string& FragmentPath = "../content/shaders/fragDefault.shaders");
	~Shader();

	void Bind() const ;
	void Unbind() const ;

	void SetUniform(const std::string& Name, int Value);
	void SetUniform(const std::string& Name, float Value);
	void SetUniform(const std::string& Name, float v0, float v1, float v2);
	void SetUniform(const std::string& Name, float v0, float v1, float v2, float v3);
	void SetUniform(const std::string& Name, const glm::mat3 & Value);
	void SetUniform(const std::string& Name, const glm::vec3& Value);
	void SetUniform(const std::string& Name, const glm::vec4& Value);
	void SetUniform(const std::string& Name, const glm::mat4& Value);
private:
	int GetUniformLocation(const std::string& Name);
	static int GetShaderLocation(unsigned int Type, const std::string& Name);
};
