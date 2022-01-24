// frontend/opengl_display_image.h - Johan Smet - BSD-3-Clause (see LICENSE)
#pragma once

#include <raytrace/types.h>

namespace rtiow {
namespace gui {

class OpenGLDisplayImage {
public:
	// construction
	OpenGLDisplayImage() = default;
	OpenGLDisplayImage(const OpenGLDisplayImage &) = delete;
	OpenGLDisplayImage(const OpenGLDisplayImage &&) = delete;
	OpenGLDisplayImage &operator=(const OpenGLDisplayImage &) = delete;

	void init(int32_t resolution_x, int32_t resolution_y);

	// display
	void display(const uint8_t *img_data, class OpenGLShader *shader);

private:
	int32_t		m_resolution_x = 0;
	int32_t		m_resolution_y = 0;

	constexpr static uint32_t INVALID = UINT32_MAX;
	uint32_t	m_texture = INVALID;
	uint32_t	m_vbo = INVALID;
	uint32_t	m_ibo = INVALID;
	uint32_t	m_vao = INVALID;
};


} // namespace rtiow::gui
} // namespace rtiow
