// raytrace/camera.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "camera.h"

namespace rtiow {

Camera::Camera( float aspect_ratio, float vertical_fov, point_t look_from, point_t look_at, vector_t v_up) {

	const auto viewport_height = 2.0f * glm::tan(glm::radians(vertical_fov) / 2.0f);
	const auto viewport_width = viewport_height * aspect_ratio;
	const auto focal_length = 1.0f;

	const auto w = glm::normalize(look_at - look_from);
	const auto u = glm::normalize(glm::cross(w, v_up));
	const auto v = glm::cross(u, w);

	m_origin = look_from;
	m_vec_horizontal = viewport_width * u;
	m_vec_vertical = viewport_height * v;
	m_lower_left = m_origin - (m_vec_horizontal / 2.0f) - (m_vec_vertical / 2.0f) + (focal_length * w);
}


} // namespace rtiow
