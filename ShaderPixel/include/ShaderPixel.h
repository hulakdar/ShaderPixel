#include "Application.h"
#include "Shader.h"
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "glm/glm.hpp"

struct Scene
{
	std::vector<int> meshes;
};

struct Environment
{
	int kek;
};

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
private:
	Shader mProgram;
	VertexBuffer		mVBO;
	VertexArray			mVAO;
	glm::mat4 mViewProjectionMatrix;
	float mRotationAngle;
};
