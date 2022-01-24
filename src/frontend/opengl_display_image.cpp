// frontend/output_opengl.h - Johan Smet - BSD-3-Clause (see LICENSE)

#include "opengl_display_image.h"
#include "opengl_shader.h"

#include <glad/glad.h>
#include <array>

namespace rtiow {

namespace gui {

namespace {

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

} // unnamed namespace

void OpenGLDisplayImage::init(int32_t resolution_x, int32_t resolution_y) {

	m_resolution_x = resolution_x;
	m_resolution_y = resolution_y;

	// create texture
	glCreateTextures(GL_TEXTURE_2D, 1, &m_texture);
	glTextureStorage2D(m_texture, 1, GL_RGB8, GLsizei(resolution_x), GLsizei(resolution_y));
	glTextureParameteri(m_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// vertex array
	// >> create and fill vertex buffer
	glCreateBuffers(1, &m_vbo);
	glNamedBufferData(m_vbo, sizeof(Vertex) * FSQ_VERTICES.size(), FSQ_VERTICES.data(), GL_STATIC_DRAW);

	// create and fill index buffer
	glCreateBuffers(1, &m_ibo);
	glNamedBufferData(m_ibo, sizeof(uint16_t) * FSQ_INDICES.size(), FSQ_INDICES.data(), GL_STATIC_DRAW);

	// create vertex array
	glCreateVertexArrays(1, &m_vao);

	// bind the vertex buffer at binding point 0
	glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, sizeof(Vertex));

	// bind the index buffer
	glVertexArrayElementBuffer(m_vao, m_ibo);

	// setup vertex attributes
	glEnableVertexArrayAttrib(m_vao, 0);
	glVertexArrayAttribFormat(m_vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, x));
	glVertexArrayAttribBinding(m_vao, 0, 0);

	glEnableVertexArrayAttrib(m_vao, 1);
    glVertexArrayAttribFormat(m_vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, u));
    glVertexArrayAttribBinding(m_vao, 1, 0);
}

void OpenGLDisplayImage::display(const uint8_t *img_data, OpenGLShader *shader) {

	assert(shader);

	glTextureSubImage2D(m_texture, 0, 0, 0,
						GLsizei(m_resolution_x), GLsizei(m_resolution_y),
						GL_RGB, GL_UNSIGNED_BYTE,
						img_data);

	glBindVertexArray(m_vao);
	glBindTextureUnit(0, m_texture);
	shader->bind();
	glDrawElements(GL_TRIANGLES, FSQ_INDICES.size(), GL_UNSIGNED_SHORT, nullptr);
}

} // namespace rtiow::gui

} // namespace rtiow
