#include "Application.h"
#include "Renderer.h"

struct AppMemory
{
	//ShaderManager	shaderManager;
	Scene			scene;
};

class ShaderPixel : public Application
{
public:
	virtual void init(Host*) override;
	virtual void deinit() override;

	virtual void update() override;
	virtual void updateWindowSize(int x, int y) override;
	virtual void renderUI() override;
	virtual void preframe() override;
	virtual void onKey(int key, int scancode, int action, int mods) override;
	virtual void onMouseMove(float x, float y, float dX, float dY) override;
	virtual ~ShaderPixel();
private:
	Shader	mDebugShader;
	float	mAspectRatio = 1.f;
	float	angle = 0.f;
	glm::vec3 CameraPosition {0,0,0};
	glm::vec2 mWindowSize{ 0,0 };
};
