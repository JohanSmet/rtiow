// frontend/output_opengl.h - Johan Smet - BSD-3-Clause (see LICENSE)

#pragma once

#include <raytrace/types.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>

namespace rtiow {

namespace gui {

class OutputOpengl {
public:
	// construction
	OutputOpengl();
	OutputOpengl(const OutputOpengl &) = delete;
	OutputOpengl(const OutputOpengl &&) = delete;
	OutputOpengl &operator=(const OutputOpengl &) = delete;
	~OutputOpengl();

	bool setup(int resolution_x, int resolution_y);
	void teardown();
	void set_active_shader(class OpenGLShader *shader);

	bool should_exit();
	void display(const uint8_t *img_data);

private:
	void draw_fullscreen_image(const uint8_t *img_data);


private:
	int m_resolution_x;
	int m_resolution_y;

	GLFWwindow *m_window;

	std::unique_ptr<class OpenGLShader>				m_default_shader;
	std::unique_ptr<class FilterGlslSmartDeNoise>	m_filter_gsd;

	class OpenGLShader *							m_active_shader;
};


} // namespace rtiow::gui

} // namespace rtiow
