#include <functional>
#include <string>
#include <vector>

struct Host;

struct Callback
{
	std::string name;
	std::function<void(Host*)> callback;
};

class Application
{
public:
	virtual void update(Host*) = 0;
	virtual void updateWindowSize(int x, int y) = 0;
	virtual void init(Host*) = 0;
	virtual void deinit(Host*) = 0;
	virtual void renderUI(Host*) = 0;
	virtual std::vector<Callback> getCallbacks() = 0;
};