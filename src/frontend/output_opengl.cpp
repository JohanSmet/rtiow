// frontend/output_opengl.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
#include "output_opengl.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "opengl_shader.h"
#include "glsl_smart_denoise.h"

#include "imgui_impl.h"

#include <cstdio>

namespace rtiow {
namespace gui {

namespace {

#ifdef NDEBUG
static constexpr bool OPENGL_DEBUG = false;
#else
static constexpr bool OPENGL_DEBUG = true;
#endif

[[maybe_unused]]
static void APIENTRY opengl_debug_callback(
						[[maybe_unused]] GLenum source,
						[[maybe_unused]] GLenum type,
						[[maybe_unused]] GLuint id,
						GLenum severity,
						[[maybe_unused]] GLsizei length,
						const GLchar* message,
						[[maybe_unused]] const void* userParam) {

	fprintf(stderr, "OPENGL: %s\n", message);

	if (severity == GL_DEBUG_SEVERITY_HIGH) {
		fprintf(stderr, "OPENGL: error has severity HIGH ==> exiting program\n");
		abort();
	}
}

void glfw_error_callback(int error, const char *description) {
	fprintf(stderr, "GLFW ERROR (%d): %s\n", error, description);
}

void glfw_key_callback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

} // unnamed namespace

OutputOpenGL::OutputOpenGL() :
		m_resolution_x(0),
		m_resolution_y(0),
		m_window(nullptr),
		m_default_shader(nullptr),
		m_filter_gsd(nullptr),
		m_active_shader(nullptr) {
}

OutputOpenGL::~OutputOpenGL() {
}

bool OutputOpenGL::setup(int resolution_x, int resolution_y) {
	m_resolution_x = resolution_x;
	m_resolution_y = resolution_y;

	// init GLFW
	glfwSetErrorCallback(rtiow::gui::glfw_error_callback);
	if (!glfwInit()) {
		return false;
	}

	// create GLFW window
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, OPENGL_DEBUG);

    m_window = glfwCreateWindow(m_resolution_x, m_resolution_y,
								   "RayTracing In One Weekend",
								   NULL, NULL);
    if (!m_window) {
        glfwTerminate();
		return false;
    }

	glfwSetKeyCallback(m_window, rtiow::gui::glfw_key_callback);

	// setup OpenGL
	glfwMakeContextCurrent(m_window);
	gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
	glfwSwapInterval(1);

	// setup OpenGL debugging
	if (OPENGL_DEBUG) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(rtiow::gui::opengl_debug_callback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
	}

	// default shader
	m_default_shader = OpenGLShader::create_from_files(
							"src/frontend/shaders/vertex.glsl",
							"src/frontend/shaders/fragment_plain.glsl");
	m_active_shader = m_default_shader.get();

	// fullscreen image drawing
	m_img_display.init(m_resolution_x, m_resolution_y);

	// setup uniform buffer for glslSmartDenoise
	m_filter_gsd = std::make_unique<FilterGlslSmartDeNoise>(this);
	m_filter_gsd->set_image_size(resolution_x, resolution_y);

	// gui
	imgui_impl_init(m_window);

	return true;
}

void OutputOpenGL::teardown() {
	imgui_impl_teardown();

	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void OutputOpenGL::set_active_shader(OpenGLShader *shader) {
	m_active_shader = (shader != nullptr) ? shader : m_default_shader.get();
}

bool OutputOpenGL::should_exit() {
	return glfwWindowShouldClose(m_window);
}

void OutputOpenGL::display(const uint8_t *img_data) {

	// ui
	imgui_impl_ui_setup();
	m_filter_gsd->imgui_config_window();
	imgui_impl_ui_finish();

	 // setup rendering
	constexpr float clear_color[4] = {0.3f, 0.3f, 0.3f, 1.0f};
	glClipControl(GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE);
	glViewportIndexedf(0, 0, 0, float(m_resolution_x), float(m_resolution_y));
	glClearNamedFramebufferfv(0, GL_COLOR, 0, clear_color);

	// dislay rendered image
	m_img_display.display(img_data, m_active_shader);

	// display UI
	imgui_impl_ui_render();

	// present
	glfwSwapBuffers(m_window);

	// event handling
	glfwPollEvents();
}

} // namespace rtiow::gui
} // namespace rtiow
