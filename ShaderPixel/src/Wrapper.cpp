#pragma once

#include "Wrapper.h"
#include <glad/glad.h>
#include <string>
#include <iostream>

void GLClearError()
{
	while (glGetError());
}

bool GLCheckError(const char *Function, const char *File, int Line)
{
	while (GLenum Error = glGetError())
	{
		std::cerr << "OpenGL error: [" << std::hex << Error << "] while calling function " << Function << "; in [" << File << "] on line [" << std::dec << Line << "]\n";
		return false;
	}
	return true;
}
