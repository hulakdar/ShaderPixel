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
	virtual ~ShaderPixel();
private:
	Shader	mDebugShader;
	float	mAspectRatio = 1.f;
};
