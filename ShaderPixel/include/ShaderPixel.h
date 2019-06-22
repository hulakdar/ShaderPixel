#include "Application.h"
#include "ShaderManager.h"

class ShaderPixel : public Application
{
	ShaderManager	mShaderManager;
public:
	virtual void update(Host*) override;
	virtual std::vector<Callback> getCallbacks() override;
	virtual void init(Host*, GLADloadproc) override;
	virtual void deinit(Host*) override;

	virtual void updateWindowSize(int x, int y) override;


	virtual void renderUI(Host*) override;


	virtual void preframe(Host*) override;

};