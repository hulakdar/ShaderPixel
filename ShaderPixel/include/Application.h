#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ShaderManager.h"

namespace Utils
{
	std::string StringFromFile(const std::string& filename);
}

struct Application
{
	Application();
	~Application();
	static Application *FromWindow(GLFWwindow* window);

	void update();
	bool shouldClose();
	void swapBuffers();
	void updateWindowSize(int x, int y);
private:
	ShaderManager	mShaderManager;
	int				mWidth, mHeight;
	GLFWwindow		*mWindow;
};
