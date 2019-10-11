#pragma once
#include "Utils.h"

#include <GLFW/glfw3.h>
#include <string>

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
	void	onMouseMove(float x, float y);
	void	onScroll(float x, float y);
	void	preframe();
	void	update();
	bool	shouldClose();
	void	swapBuffers();
	void	updateWindowSize(int x, int y);

	float		 mScale;

	AppMemory	*mMemory;
	GLFWwindow		*mWindow;

	// allocators for DLL
	void	*(*allocate)(size_t);
	void	(*deallocate)(void *);
private:
	void			*mLib;
	std::string		mDllPath;
	Timestamp		mDllTimestamp;
	Application		*mApplication;
	int				mWidth, mHeight;
	float			mMouseX, mMouseY;
	bool			mForceReload;

	void			updateDLL();
};
