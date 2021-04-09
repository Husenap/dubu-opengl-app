#pragma once

#include <dubu_window/dubu_window.h>

namespace dubu::opengl_app {

class AppBase {
public:
	struct CreateInfo {
		std::string appName = "dubu-opengl-app";
		int swapInterval = 0;
	};

public:
	AppBase(const CreateInfo& createInfo);
	virtual ~AppBase() = default;

	void Run();

protected:
	virtual void Init()   = 0;
	virtual void Update() = 0;

	std::unique_ptr<dubu::window::GLFWWindow> mWindow;

private:
	CreateInfo mCreateInfo;
};

}  // namespace dubu::opengl_app