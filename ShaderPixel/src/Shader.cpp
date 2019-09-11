#include "Shader.h"
#include "Renderer.h"
#include "Utility.h"
#include "Resources.h"

#include <string>
#include <fstream>
#include <iostream>
#include <glad/glad.h>

static unsigned int sCurrentlyBound = 0;

static std::map<std::string, int> sShaderCache;

static std::map<FeatureMask, ShaderID> sPermutations;

static std::string GetShaderSource(const std::string& Filepath, const std::string& Modifier)
{
	// temporary string for holding line from file
	std::string tmp;

	std::ifstream ShaderFile(Filepath);
	std::string ShaderSource;

	if (!std::getline(ShaderFile, tmp))
		std::cerr << "Problems reading shader from: " << Filepath << '\n';

	ShaderSource += tmp + "\n" + Modifier;

	while (std::getline(ShaderFile, tmp))
		ShaderSource += tmp + "\n";

	return ShaderSource;
}

static int CompileShader(unsigned int Type, const std::string& Filepath, const std::string& Modifier)
{
	std::cerr << Filepath << " is compiling\n";
	const std::string ShaderSourceTmp = GetShaderSource(Filepath, Modifier);
	GLCall(unsigned int ShaderProgram = glCreateShader(Type));
	const char *str = ShaderSourceTmp.c_str();
	GLCall(glShaderSource(ShaderProgram, 1, &str, 0));
	GLCall(glCompileShader(ShaderProgram));

	int result;
	GLCall(glGetShaderiv(ShaderProgram, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE)
	{
		GLCall(glGetShaderiv(ShaderProgram, GL_INFO_LOG_LENGTH, &result));
		char *message = static_cast<char *>(alloca(result));
		GLCall(glGetShaderInfoLog(ShaderProgram, result, &result, message));
		std::cerr << "Failed to compile " << (Type == GL_FRAGMENT_SHADER ? "fragment" : "vertex") << " shader:" << message << std::endl;
		GLCall(glDeleteShader(ShaderProgram));
		__debugbreak();
		return (0);
	}
	return ShaderProgram;
}

static int GetOrCompileShader(unsigned int Type, const std::string& Name, const std::string& Modifier)
{
//	auto FindIterator = sShaderCache.find(Name);
//	if (FindIterator != sShaderCache.end())
//		return FindIterator->second;
	unsigned int Location = CompileShader(Type, Name, Modifier);
	//sShaderCache[Name] = Location;
	return Location;
}

static unsigned int CreateShader(unsigned int Vs, unsigned int Fs)
{
	GLCall(unsigned int Program = glCreateProgram());
	GLCall(glAttachShader(Program, Vs));
	GLCall(glAttachShader(Program, Fs));

	{
		char errText[512];
		GLCall(glLinkProgram(Program));
		int success;
		glGetProgramiv(Program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(Program, 512, nullptr, errText);
			std::cout << errText << '\n';
			__debugbreak();
		}
	}

	GLCall(glValidateProgram(Program));
	return Program;
}

Shader::Shader(
	const std::string& VertexPath,
	const std::string& FragmentPath,
	const std::string& Modifier /*= ""*/)
{
	unsigned int VertexShader = GetOrCompileShader(GL_VERTEX_SHADER, VertexPath, Modifier);
	unsigned int FragmentShader = GetOrCompileShader(GL_FRAGMENT_SHADER, FragmentPath, Modifier);

	mRendererID = CreateShader(VertexShader, FragmentShader);
	Bind();
}

Shader::Shader(Shader&& Other)
{
	*this = std::move(Other);
}

static std::string ModifierFromMask(FeatureMask mask)
{
	const static std::string Modifiers[] = {
		"#define ALPHA_TEXTURE 1\n",
		"#define MASKED 1\n",
		"#define DITHERED 1\n",
	};

	static_assert(ARRAY_COUNT(Modifiers) == Feature::Count);

	std::string result;
	for (int i = 0; i < ARRAY_COUNT(Modifiers); i++)
		if ((1 << i) & mask)
			result += Modifiers[i];

	return result;
}

Shader::Shader(FeatureMask mask) : Shader(
"../content/shaders/vertDefault.shader",
"../content/shaders/fragDefault.shader",
ModifierFromMask(mask))
{ }

Shader& Shader::operator=(Shader&& Other)
{
	mRendererID = Other.mRendererID;
	std::swap(Other.mLocationCache, mLocationCache);
	return *this;
}

Shader::~Shader()
{
	//GLCall(glDeleteProgram(mRendererID));
}

void Shader::Bind() const
{
	//if (mRendererID == s_CurrentlyBound)
	//	return;
	GLCall(glUseProgram(mRendererID));
	//sCurrentlyBound = mRendererID;
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
	//sCurrentlyBound = 0;
}

void Shader::SetUniform(const std::string & Name, int value)
{
	Bind();
	GLCall(glUniform1i(GetUniformLocation(Name), value));
}

void Shader::SetUniform(const std::string & Name, unsigned int value)
{
	Bind();
	GLint location = GetUniformLocation(Name);
	if (location > 0)
		GLCall(glUniform1ui(location, value));
}

void Shader::SetUniform(const std::string & Name, float value)
{
	Bind();
	GLCall(glUniform1f(GetUniformLocation(Name), value));
}

void Shader::SetUniform(const std::string & Name, float v0, float v1)
{
	Bind();
	GLCall(glUniform2f(GetUniformLocation(Name), v0, v1));
}

void Shader::SetUniform(const std::string & Name, const glm::vec2& value)
{
	Bind();
	GLCall(glUniform2fv(GetUniformLocation(Name), 1, &value[0]));
}

void Shader::SetUniform(const std::string & Name, float v0, float v1, float v2)
{
	Bind();
	GLCall(glUniform3f(GetUniformLocation(Name), v0, v1, v2));
}

void Shader::SetUniform(const std::string & Name, const glm::vec3& value)
{
	Bind();
	GLCall(glUniform3fv(GetUniformLocation(Name), 1, &value[0]));
}

void Shader::SetUniform(const std::string & Name, const glm::vec4& value)
{
	Bind();
	GLCall(glUniform4fv(GetUniformLocation(Name), 1, &value[0]));
}

void Shader::SetUniform(const std::string & Name, float v0, float v1, float v2, float v3)
{
	Bind();
	GLCall(glUniform4f(GetUniformLocation(Name), v0, v1, v2, v3));
}

void Shader::SetUniform(const std::string & Name, const glm::mat3& Value)
{
	Bind();
	GLCall(glUniformMatrix3fv(GetUniformLocation(Name), 1, GL_FALSE, &Value[0][0]));
}

void Shader::SetUniform(const std::string & Name, const glm::mat4& Value)
{
	Bind();
	GLCall(glUniformMatrix4fv(GetUniformLocation(Name), 1, GL_FALSE, &Value[0][0]));
}

void Shader::SetUniform(const Uniform& Uniform)
{
	switch (Uniform.type)
	{
	case UniformType::FLOAT: {
		SetUniform(Uniform.name, Uniform.scalar);
		break;
	}
	case UniformType::VEC2: {
		SetUniform(Uniform.name, Uniform.v2);
		break;
	}
	case UniformType::VEC3: {
		SetUniform(Uniform.name, Uniform.v3);
		break;
	}
	case UniformType::VEC4: {
		SetUniform(Uniform.name, Uniform.v4);
		break;
	}
	case UniformType::IVEC2: {
		SetUniform(Uniform.name, Uniform.iv2);
		break;
	}
	case UniformType::IVEC3: {
		SetUniform(Uniform.name, Uniform.iv3);
		break;
	}
	case UniformType::IVEC4: {
		SetUniform(Uniform.name, Uniform.iv4);
		break;
	}
	case UniformType::TEX: {
		TextureBinding texBinding = Uniform.tex;
		Texture *tex = Resources::GetTexture(texBinding.id);
		tex->Bind(texBinding.usage);
		SetUniform(Uniform.name, (GLint)texBinding.usage);
		break;
	}
	case UniformType::CODE: {
		CustomCode code = Uniform.code;
		if (code.callback)
			code.callback(this, code.payload);
	}
	default:
		break;
	}
}

void Shader::SetUniform(const std::string& Name, const glm::ivec2& value)
{
	Bind();
	GLCall(glUniform2iv(GetUniformLocation(Name), 1, &value[0]));
}

void Shader::SetUniform(const std::string& Name, const glm::ivec3& value)
{
	Bind();
	GLCall(glUniform3iv(GetUniformLocation(Name), 1, &value[0]));
}

void Shader::SetUniform(const std::string& Name, const glm::ivec4& value)
{
	Bind();
	GLCall(glUniform4iv(GetUniformLocation(Name), 1, &value[0]));
}

ShaderID Shader::GetShaderWithFeatures(FeatureMask mask)
{
	auto It = sPermutations.find(mask);
	if (It != sPermutations.end())
		return It->second;

	ShaderID shaderID = Resources::Shaders.size();
	Resources::Shaders.emplace_back(mask);
	sPermutations[mask] = shaderID;
	return shaderID;
}

int Shader::GetUniformLocation(const std::string & Name)
{
	auto FindIterator = mLocationCache.find(Name);
	if (FindIterator != mLocationCache.end())
		return FindIterator->second;
	GLCall(int Location = glGetUniformLocation(mRendererID, Name.c_str()));
	mLocationCache[Name] = Location;
	if (Location == -1)
		std::cerr << Name << " wasn't found as a uniform\n";
	return Location;
}

