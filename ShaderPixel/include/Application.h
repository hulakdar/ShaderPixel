#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>


struct Application
{
	Application();
	~Application();

	static Application *FromWindow(GLFWwindow* window);

	void updateWindowSize(int x, int y);
	bool shouldClose();
	void swapBuffers();
	void update();
private:
	GLFWwindow	*mWindow;
	int mWidth, mHeight;
};
