#include "Application.h"
#include "Renderer.h"
#include "RenderTarget.h"

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
	virtual void onScroll(float x, float y) override;
	virtual ~ShaderPixel();
private:
	ShaderID	mBox;
	ShaderID	mFXAA;
	ShaderID	mCloud;
	ShaderID	mFullscreenTest;

	RenderTarget mSceneColorMS;
	RenderTarget mSceneColor;
	RenderTarget mPostProcess;
	RenderTarget mShadow;

	float		mAspectRatio = 1.f;
	float		mSpeed = 1.f;

	glm::vec3	mLightAngles	{ 1., 1.5, 0. };
	glm::vec3	mCameraPosition {0,10,50};
	glm::vec3	mCameraForward	{0,0,1};
	glm::vec3	mCameraRight	{1,0,0};
	glm::vec3	mCameraUp		{0,1,0};
	glm::vec2	mCameraAngles	{ 0,0 };

	glm::vec2	mWindowSize		{ 0,0 };
};
