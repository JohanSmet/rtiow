// frontend/output_opengl.h - Johan Smet - BSD-3-Clause (see LICENSE)

#pragma once

#include <raytrace/types.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace rtiow {

namespace gui {

class OutputOpengl {
public:
	// construction
	OutputOpengl() = default;
	OutputOpengl(const OutputOpengl &) = delete;
	OutputOpengl(const OutputOpengl &&) = delete;
	OutputOpengl &operator=(const OutputOpengl &) = delete;

	bool setup(int resolution_x, int resolution_y);
	void teardown();

	bool should_exit();
	void display(const uint8_t *img_data);

private:
	int m_resolution_x;
	int m_resolution_y;

	GLFWwindow *m_window;
};


} // namespace rtiow::gui

} // namespace rtiow
