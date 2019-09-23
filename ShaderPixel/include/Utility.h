#pragma once

#define ARRAY_COUNT(x) (sizeof(x) / sizeof(x[0]))

#define GLCall(x) \
	GLClearError();\
	x;\
	GLCheckError(#x, __FILE__, __LINE__)

void GLClearError();

bool GLCheckError(const char *Function, const char *File, int Line);
