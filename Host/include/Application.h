#include <string>
#include <vector>
#include <glad/glad.h>

struct Host;
Host& getHost();

class Application
{
public:
	virtual void init(Host*) = 0;
	virtual void deinit() = 0;

	virtual void preframe() = 0;
	virtual void update() = 0;
	virtual void onKey(int key, int scancode, int action, int mods) = 0;
	virtual void onMouseMove(float x, float y, float dX, float dY) = 0;
	virtual void updateWindowSize(int x, int y) = 0;
	virtual void renderUI() = 0;
	virtual ~Application() {}
};

typedef Application *ApplicationGetter();