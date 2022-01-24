// frontend/imgui_impl.h - Johan Smet - BSD-3-Clause (see LICENSE)

#pragma once

struct GLFWwindow;

namespace rtiow {
namespace gui {

void imgui_impl_init(GLFWwindow *window);
void imgui_impl_teardown();

void imgui_impl_ui_setup();
void imgui_impl_ui_finish();

void imgui_impl_ui_render();

} // namespace rtiow::gui
} // namespace rtiow
