#pragma once

#include <string>
#include <glm/glm.hpp>
#include <map>
#include "Uniform.h"

typedef size_t ShaderID;

typedef uint8_t FeatureMask;
enum Feature {
	AlphaTexture = 1 << 0,
	Masked = 1 << 1,
	Dithered = 1 << 2,
	Count = 3 // update by hand?? not cool :C
};

class Shader
{
	unsigned int				mRendererID;
	std::map<std::string, int>	mLocationCache;
public:
	Shader(FeatureMask mask);
	static ShaderID GetShaderWithFeatures(FeatureMask mask);

	Shader(const std::string& VertexPath,
		const std::string& FragmentPath,
		const std::string& Modifier = "" //if you pass something here, don't forget to add  \n
		);
	Shader(const Shader& Other) = delete;
	Shader& operator=(const Shader& Other) = delete;

	Shader(Shader&& Other);
	Shader& operator=(Shader&& Other);
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
};
