#pragma once
#include <GLFW/glfw3.h>
#include <string>

namespace Utils
{
	std::string StringFromFile(const std::string& filename);
}

class Application;

struct Host
{
	Host();
	~Host();
	static Host *FromWindow(GLFWwindow* window);

		
	void	preframe();
	void	update();
	bool	shouldClose();
	void	swapBuffers();
	void	updateWindowSize(int x, int y);

	float	mScale;
private:
	int				mWidth, mHeight;
	GLFWwindow		*mWindow;
	Application		*mApplication;
};
