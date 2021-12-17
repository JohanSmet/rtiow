// raytrace/camera.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "camera.h"

namespace rtiow {

Camera::Camera(uint32_t img_width, uint32_t img_height) {

	const auto aspect_ratio = static_cast<float> (img_width) / static_cast<float>(img_height);
	const auto viewport_height = 2.0f;
	const auto viewport_width = viewport_height * aspect_ratio;
	const auto focal_length = 1.0f;

	m_origin = point_t(0.0f, 0.0f, 0.0f);
	m_vec_horizontal = vector_t(viewport_width, 0, 0);
	m_vec_vertical = vector_t(0, viewport_height, 0);
	m_lower_left = m_origin - (m_vec_horizontal / 2.0f) - (m_vec_vertical / 2.0f) - vector_t(0, 0, focal_length);
}


} // namespace rtiow
