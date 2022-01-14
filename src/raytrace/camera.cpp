// raytrace/camera.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "camera.h"
#include "raytrace/utils.h"

namespace rtiow {

Camera::Camera( float aspect_ratio, float vertical_fov,
				point_t look_from, point_t look_at, vector_t v_up,
				float aperture, float focus_distance) {

	const auto viewport_height = 2.0f * glm::tan(glm::radians(vertical_fov) / 2.0f);
	const auto viewport_width = viewport_height * aspect_ratio;

	m_w = glm::normalize(look_at - look_from);
	m_u = glm::normalize(glm::cross(m_w, v_up));
	m_v = glm::cross(m_u, m_w);

	if (glm::epsilonEqual(focus_distance, 0.0f, 0.000001f)) {
		focus_distance = glm::length(look_at - look_from);
	}

	m_origin = look_from;
	m_vec_horizontal = focus_distance * viewport_width * m_u;
	m_vec_vertical = focus_distance * viewport_height * m_v;
	m_lower_left = m_origin - (m_vec_horizontal / 2.0f) - (m_vec_vertical / 2.0f) + (focus_distance * m_w);

	m_lens_radius = aperture / 2.0f;
}

Ray Camera::create_ray(float s, float t) const {
	auto rd = m_lens_radius * random_vector_in_unit_disc();
	auto offset = m_u * rd.x + m_v * rd.y;
	return Ray(
		m_origin + offset,
		glm::normalize(m_lower_left + (s * m_vec_horizontal) + (t * m_vec_vertical) - m_origin - offset)
	);
}


} // namespace rtiow
