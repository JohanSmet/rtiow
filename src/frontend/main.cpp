
#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <thread>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <raytrace/raytrace.h>

constexpr static uint32_t RESOLUTION_X = 640;
constexpr static uint32_t RESOLUTION_Y = 360;

constexpr static bool OPENGL_DEBUG = true;

static constexpr int MAX_FPS = 60;
static constexpr int MIN_FRAMETIME_MS = 1000 / MAX_FPS;

namespace rtiow {

namespace gui {

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

static constexpr const GLchar *FSQ_VERTEX_SHADER =
R"(#version 450
	layout (location = 0) in vec3 in_position;
	layout (location = 1) in vec2 in_tex_coord;

	out vec2 vs_tex_coord;

	void main() {
		gl_Position = vec4(in_position, 1.0f);
		vs_tex_coord = in_tex_coord;
	}
)";

static constexpr const GLchar *FSQ_FRAGMENT_SHADER =
R"(#version 450
	in vec2 vs_tex_coord;
	layout (binding = 0) uniform sampler2D tex;

	layout (location = 0) out vec4 out_color;

	void main() {
		out_color = texture(tex, vs_tex_coord);
	}
)";

static GLuint g_texture = GL_INVALID_INDEX;
static GLuint g_vbo = GL_INVALID_INDEX;
static GLuint g_ibo = GL_INVALID_INDEX;
static GLuint g_vao = GL_INVALID_INDEX;
static GLuint g_vertex_shader = GL_INVALID_INDEX;
static GLuint g_fragment_shader = GL_INVALID_INDEX;
static GLuint g_shader = GL_INVALID_INDEX;

void gl_setup_fullscreen_quad() {

	// create texture
	glCreateTextures(GL_TEXTURE_2D, 1, &g_texture);
	glTextureStorage2D(g_texture, 1, GL_RGB8, RESOLUTION_X, RESOLUTION_Y);
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
	glShaderSource(g_vertex_shader, 1, &FSQ_VERTEX_SHADER, nullptr);
	glCompileShader(g_vertex_shader);

	// >> fragment shader
	g_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(g_fragment_shader, 1, &FSQ_FRAGMENT_SHADER, nullptr);
	glCompileShader(g_fragment_shader);

	// >> program
	g_shader = glCreateProgram();
	glAttachShader(g_shader, g_vertex_shader);
	glAttachShader(g_shader, g_fragment_shader);
	glLinkProgram(g_shader);
}

void gl_display_fullscreen_quad(const uint8_t *img_data) {

	glTextureSubImage2D(g_texture, 0, 0, 0, RESOLUTION_X, RESOLUTION_Y, GL_RGB, GL_UNSIGNED_BYTE, img_data);

	glBindVertexArray(g_vao);
	glBindTextureUnit(0, g_texture);
	glUseProgram(g_shader);
	glDrawElements(GL_TRIANGLES, FSQ_INDICES.size(), GL_UNSIGNED_SHORT, nullptr);
}

} // namespace rtiow::gui

} // namespace rtiow

int main() {

	// init GLFW
	glfwSetErrorCallback(rtiow::gui::glfw_error_callback);
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	// create GLFW window
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, OPENGL_DEBUG);

    auto window = glfwCreateWindow(RESOLUTION_X, RESOLUTION_Y, "RayTracing In One Weekend", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

	glfwSetKeyCallback(window, rtiow::gui::glfw_key_callback);

	// setup OpenGL
	glfwMakeContextCurrent(window);
	gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
	glfwSwapInterval(1);

	// setup OpenGL debugging
	if (OPENGL_DEBUG) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(rtiow::gui::opengl_debug_callback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
	}

	rtiow::gui::gl_setup_fullscreen_quad();

	// create scene
	rtiow::Scene scene;
	auto material_ground = scene.material_create(rtiow::color_t(0.8f, 0.8f, 0.0f));
	auto material_center = scene.material_create(rtiow::color_t(0.7f, 0.3f, 0.3f));

	scene.sphere_add(rtiow::point_t(0.0f, -100.5f, -1.0f), 100.0f, material_ground);
	scene.sphere_add(rtiow::point_t(0.0f, 0.0f, -1.0f), 0.5f, material_center);

	// kick of renderer
	rtiow::RayTracer ray_tracer(RESOLUTION_X, RESOLUTION_Y);

	std::thread render_thread([&ray_tracer, &scene] {
		ray_tracer.render(scene);
	});

	while (!glfwWindowShouldClose(window)) {

		auto clock_start = clock();

		 // setup rendering
		constexpr float clear_color[4] = {0.3f, 0.3f, 0.3f, 1.0f};
		glClipControl(GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE);
		glViewportIndexedf(0, 0, 0, RESOLUTION_X, RESOLUTION_Y);
		glClearNamedFramebufferfv(0, GL_COLOR, 0, clear_color);

		// dislay rendered image
		rtiow::gui::gl_display_fullscreen_quad(ray_tracer.output_ptr());

		// present
		glfwSwapBuffers(window);

		// event handling
		glfwPollEvents();

		// fps limiter
		auto clock_end = clock();
		auto sleep_time = MIN_FRAMETIME_MS - ((clock_end - clock_start) * 1000) / CLOCKS_PER_SEC;
		if (sleep_time > 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
		}
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
