// frontend/output_opengl.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
#include "output_opengl.h"

#include <array>
#include <cstdio>
#include <iterator>
#include <string>
#include <fstream>

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

// full screen quad
struct Vertex {
	float x, y, z;      // position
	float u, v;			// texture coordinates
};

static constexpr std::array<Vertex, 4> FSQ_VERTICES {
	Vertex{ 1.0f,  1.0f, 0.0f, 1.0f, 1.0f},
	Vertex{ 1.0f, -1.0f, 0.0f, 1.0f, 0.0f},
	Vertex{-1.0f, -1.0f, 0.0f, 0.0f, 0.0f},
	Vertex{-1.0f,  1.0f, 0.0f, 0.0f, 1.0f}
};

static constexpr std::array<uint16_t, 6> FSQ_INDICES {
	0, 1, 3,
	1, 2, 3
};

static GLuint g_texture = GL_INVALID_INDEX;
static GLuint g_vbo = GL_INVALID_INDEX;
static GLuint g_ibo = GL_INVALID_INDEX;
static GLuint g_vao = GL_INVALID_INDEX;
static GLuint g_vertex_shader = GL_INVALID_INDEX;
static GLuint g_fragment_shader = GL_INVALID_INDEX;
static GLuint g_shader = GL_INVALID_INDEX;

void gl_load_shader_source(GLuint shader, const char *file_path) {

	std::ifstream file(file_path);
	if (!file.is_open()) {
		fprintf(stderr, "Error opening shader source (%s)\n", file_path);
		exit(-1);
	}

	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	const GLchar *shader_source = content.c_str();

	glShaderSource(shader, 1, &shader_source, nullptr);
}

void gl_setup_fullscreen_quad(int32_t resolution_x, int32_t resolution_y) {

	// create texture
	glCreateTextures(GL_TEXTURE_2D, 1, &g_texture);
	glTextureStorage2D(g_texture, 1, GL_RGB8, GLsizei(resolution_x), GLsizei(resolution_y));
	glTextureParameteri(g_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(g_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// vertex array
	// >> create and fill vertex buffer
	glCreateBuffers(1, &g_vbo);
	glNamedBufferData(g_vbo, sizeof(Vertex) * FSQ_VERTICES.size(), FSQ_VERTICES.data(), GL_STATIC_DRAW);

	// create and fill index buffer
	glCreateBuffers(1, &g_ibo);
	glNamedBufferData(g_ibo, sizeof(uint16_t) * FSQ_INDICES.size(), FSQ_INDICES.data(), GL_STATIC_DRAW);

	// create vertex array
	glCreateVertexArrays(1, &g_vao);

	// bind the vertex buffer at binding point 0
	glVertexArrayVertexBuffer(g_vao, 0, g_vbo, 0, sizeof(Vertex));

	// bind the index buffer
	glVertexArrayElementBuffer(g_vao, g_ibo);

	// setup vertex attributes
	glEnableVertexArrayAttrib(g_vao, 0);
	glVertexArrayAttribFormat(g_vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, x));
	glVertexArrayAttribBinding(g_vao, 0, 0);

	glEnableVertexArrayAttrib(g_vao, 1);
    glVertexArrayAttribFormat(g_vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, u));
    glVertexArrayAttribBinding(g_vao, 1, 0);

	// shaders
	// >> vertex shader
	g_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	gl_load_shader_source(g_vertex_shader, "src/frontend/shaders/vertex.glsl");
	glCompileShader(g_vertex_shader);

	// >> fragment shader
	g_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	gl_load_shader_source(g_fragment_shader, "src/frontend/shaders/fragment_plain.glsl");
	glCompileShader(g_fragment_shader);

	// >> program
	g_shader = glCreateProgram();
	glAttachShader(g_shader, g_vertex_shader);
	glAttachShader(g_shader, g_fragment_shader);
	glLinkProgram(g_shader);
}

void gl_display_fullscreen_quad(const uint8_t *img_data, int32_t resolution_x, int32_t resolution_y) {

	glTextureSubImage2D(g_texture, 0, 0, 0,
						GLsizei(resolution_x), GLsizei(resolution_y),
						GL_RGB, GL_UNSIGNED_BYTE,
						img_data);

	glBindVertexArray(g_vao);
	glBindTextureUnit(0, g_texture);
	glUseProgram(g_shader);
	glDrawElements(GL_TRIANGLES, FSQ_INDICES.size(), GL_UNSIGNED_SHORT, nullptr);
}

} // unnamed namespace

bool OutputOpengl::setup(int resolution_x, int resolution_y) {
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

	rtiow::gui::gl_setup_fullscreen_quad(m_resolution_x, m_resolution_y);

	return true;
}

void OutputOpengl::teardown() {
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

bool OutputOpengl::should_exit() {
	return glfwWindowShouldClose(m_window);
}

void OutputOpengl::display(const uint8_t *img_data) {

	 // setup rendering
	constexpr float clear_color[4] = {0.3f, 0.3f, 0.3f, 1.0f};
	glClipControl(GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE);
	glViewportIndexedf(0, 0, 0, float(m_resolution_x), float(m_resolution_y));
	glClearNamedFramebufferfv(0, GL_COLOR, 0, clear_color);

	// dislay rendered image
	rtiow::gui::gl_display_fullscreen_quad(img_data, m_resolution_x, m_resolution_y);

	// present
	glfwSwapBuffers(m_window);

	// event handling
	glfwPollEvents();
}

} // namespace rtiow::gui

} // namespace rtiow
