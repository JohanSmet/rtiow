// frontend/opengl_uniform_buffer.h - Johan Smet - BSD-3-Clause (see LICENSE)
#pragma once

#include <raytrace/types.h>

namespace rtiow {

class OpenGLUniformBuffer {
public:
	// construction
	OpenGLUniformBuffer() = default;
	~OpenGLUniformBuffer();

	void init(size_t buffer_size, uint32_t binding);

	// update buffer data
	void update(const void *data, size_t offset, size_t length);

	template <typename T>
	void update(const T &data) {
		update(&data, 0, sizeof(T));
	}

private:
	constexpr static uint32_t INVALID = UINT32_MAX;
	uint32_t m_id = INVALID;
};

} // namespace rtiow

