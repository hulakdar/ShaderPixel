#include "Shader.h"
#include "Renderer.h"
#include <string>
#include <fstream>
#include <iostream>
#include "Wrapper.h"
#include <glad/glad.h>

std::unordered_map<std::string, int> Shader::s_ShaderCache;
unsigned int Shader::s_CurrentlyBound = 0;

inline static std::string GetShaderSource(const std::string& Filepath)
{
	// temporary string for holding line from file
	std::string tmp;

	std::ifstream ShaderFile(Filepath);
	std::string ShaderSource;
	while (std::getline(ShaderFile, tmp))
		ShaderSource += tmp + "\n";

	return ShaderSource;
}

inline static int CompileShader(unsigned int Type, const std::string& Filepath)
{
	std::cerr << Filepath << " is compiling\n";
	const std::string ShaderSourceTmp = GetShaderSource(Filepath);
	GLCall(unsigned int Shader = glCreateShader(Type));
	const char *str = ShaderSourceTmp.c_str();
	GLCall(glShaderSource(Shader, 1, &str, 0));
	GLCall(glCompileShader(Shader));

	int result;
	GLCall(glGetShaderiv(Shader, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE)
	{
		GLCall(glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &result));
		char *message = static_cast<char *>(alloca(result));
		GLCall(glGetShaderInfoLog(Shader, result, &result, message));
		std::cerr << "Failed to compile " << (Type == GL_FRAGMENT_SHADER ? "fragment" : "vertex") << " shader:" << message << std::endl;
		GLCall(glDeleteShader(Shader));
		__debugbreak();
		return (0);
	}
	return Shader;
}

inline static unsigned int CreateShader(unsigned int Vs, unsigned int Fs)
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

Shader::Shader(const std::string & VertexPath, const std::string & FragmentPath)
{
	unsigned int VertexShader = GetShaderLocation(GL_VERTEX_SHADER, VertexPath);
	unsigned int FragmentShader = GetShaderLocation(GL_FRAGMENT_SHADER, FragmentPath);

	m_RendererID = CreateShader(VertexShader, FragmentShader);
	Bind();
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
}

void Shader::Bind() const
{
	//if (m_RendererID == s_CurrentlyBound)
	//	return;
	GLCall(glUseProgram(m_RendererID));
	s_CurrentlyBound = m_RendererID;
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
	s_CurrentlyBound = 0;
}

void Shader::SetUniform(const std::string & Name, int value)
{
	Bind();
	GLCall(glUniform1i(GetUniformLocation(Name), value));
}

void Shader::SetUniform(const std::string & Name, float value)
{
	Bind();
	GLCall(glUniform1f(GetUniformLocation(Name), value));
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

int Shader::GetUniformLocation(const std::string & Name)
{
	auto FindIterator = LocationCache.find(Name);
	if (FindIterator != LocationCache.end())
		return FindIterator->second;
	GLCall(int Location = glGetUniformLocation(m_RendererID, Name.c_str()));
	LocationCache[Name] = Location;
	if (Location == -1)
		std::cerr << Name << " wasn't found as a uniform\n";
	return Location;
}

int Shader::GetShaderLocation(unsigned int Type, const std::string & Name)
{
	auto FindIterator = s_ShaderCache.find(Name);
	if (FindIterator != s_ShaderCache.end())
		return FindIterator->second;
	unsigned int Location = CompileShader(Type, Name);
	s_ShaderCache[Name] = Location;
	return Location;
}
