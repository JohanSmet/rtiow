// frontend/output_opengl.h - Johan Smet - BSD-3-Clause (see LICENSE)

#pragma once

#include <raytrace/types.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>

#include "opengl_display_image.h"

namespace rtiow {

namespace gui {

class OutputOpenGL {
public:
	// construction
	OutputOpenGL();
	OutputOpenGL(const OutputOpenGL &) = delete;
	OutputOpenGL(const OutputOpenGL &&) = delete;
	OutputOpenGL &operator=(const OutputOpenGL &) = delete;
	~OutputOpenGL();

	bool setup(int resolution_x, int resolution_y);
	void teardown();
	void set_active_shader(class OpenGLShader *shader);

	bool should_exit();
	void display(const uint8_t *img_data);

private:
	int m_resolution_x;
	int m_resolution_y;

	GLFWwindow *m_window;

	std::unique_ptr<class OpenGLShader>				m_default_shader;
	std::unique_ptr<class FilterGlslSmartDeNoise>	m_filter_gsd;
	OpenGLDisplayImage								m_img_display;

	class OpenGLShader *							m_active_shader;
};


} // namespace rtiow::gui

} // namespace rtiow
