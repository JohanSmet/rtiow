// frontend/imgui_impl.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "imgui_impl.h"

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

namespace rtiow {
namespace gui {

void imgui_impl_init(GLFWwindow *window) {

    // setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // setup Dear ImGui style
    ImGui::StyleColorsDark();

    // setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

void imgui_impl_teardown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void imgui_impl_ui_setup() {
	// start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void imgui_impl_ui_finish() {
	ImGui::Render();
}

void imgui_impl_ui_render() {
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

} // namespace rtiow::gui
} // namespace rtiow
