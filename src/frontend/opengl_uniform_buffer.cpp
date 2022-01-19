// frontend/opengl_uniform_buffer.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "opengl_uniform_buffer.h"

#include <glad/glad.h>

namespace rtiow {

void OpenglUniformBuffer::init(size_t buffer_size, uint32_t binding) {
	glCreateBuffers(1, &m_id);
	glNamedBufferData(m_id, static_cast<GLsizeiptr>(buffer_size), nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_id);
}

OpenglUniformBuffer::~OpenglUniformBuffer() {
	if (m_id != INVALID) {
		glDeleteBuffers(1, &m_id);
	}
}

void OpenglUniformBuffer::update(const void *data, size_t offset, size_t length) {
	glNamedBufferSubData(m_id, static_cast<GLintptr>(offset), static_cast<GLsizei>(length), data);
}

} // namespace rtiow

