#pragma once
#include <GLFW/glfw3.h>
#include <string>
#include "Utils.h"

class Application;
struct AppMemory;

struct DynamicLib
{

};

struct Host
{
	Host(const std::string &dllPath);
	~Host();
	static Host *FromWindow(GLFWwindow* window);

	void	onKey(int key, int scancode, int action, int mods);
	void	preframe();
	void	update();
	bool	shouldClose();
	void	swapBuffers();
	void	updateWindowSize(int x, int y);

	float		 mScale;
	AppMemory	*mMemory;

	// allocators for DLL
	void	*(*allocate)(size_t);
	void	(*deallocate)(void *);
private:
	void			*mLib;
	std::string		mDllPath;
	Application		*mApplication;
	Timestamp		mDllTimestamp;
	int				mWidth, mHeight;
	GLFWwindow		*mWindow;

	void			updateDLL();
};
