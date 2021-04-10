#include "AppBase.hpp"

#include <glad/gl.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

namespace dubu::opengl_app {

AppBase::AppBase(const CreateInfo& createInfo)
    : mCreateInfo(createInfo) {
	dubu::log::Register<dubu::log::ConsoleLogger>();
}

void AppBase::Run() {
	mWindow = std::move(std::make_unique<dubu::window::GLFWWindow>(
	    dubu::window::GLFWWindow::CreateInfo{
	        .width  = mCreateInfo.width,
	        .height = mCreateInfo.height,
	        .title  = mCreateInfo.appName,
	        .api    = dubu::window::GLFWWindow::Api::OpenGL,
	    }));

	glfwMakeContextCurrent(mWindow->GetGLFWHandle());
	if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
		DUBU_LOG_FATAL("Failed to init glad");
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding              = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplGlfw_InitForOpenGL(mWindow->GetGLFWHandle(), true);
	ImGui_ImplOpenGL3_Init("#version 130");

	auto resizeToken = mWindow->RegisterListener<dubu::window::EventResize>(
	    [](const auto& e) { glViewport(0, 0, e.width, e.height); });
	auto keyPressToken = mWindow->RegisterListener<dubu::window::EventKeyPress>(
	    [&](const auto& e) {
		    if (e.key == GLFW_KEY_ESCAPE) {
			    glfwSetWindowShouldClose(mWindow->GetGLFWHandle(), GLFW_TRUE);
		    }
	    });

	glViewport(0, 0, mCreateInfo.width, mCreateInfo.height);
	glfwSwapInterval(mCreateInfo.swapInterval);

	Init();

	while (!mWindow->ShouldClose()) {
		mWindow->PollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		Update();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		mWindow->SwapBuffers();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

}  // namespace dubu::opengl_app
