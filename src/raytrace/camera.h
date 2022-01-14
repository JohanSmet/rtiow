// raytrace/camera.h - Johan Smet - BSD-3-Clause (see LICENSE)
#pragma once

#include "ray.h"

namespace rtiow {

class Camera {
public:
	// construction
	Camera( float aspect_ratio = 16.0f / 9.0f,
			float vertical_fov = 90.0f,
			point_t look_from = {0.0f, 0.0f, 0.0f},
			point_t look_at = {0.0f, 0.0f, -1.0f},
			vector_t v_up = {0.0f, 1.0f, 0.0f},
			float aperture = 0.0f,
			float focus_distance = 0.0f);

	// ray generation
	Ray create_ray(float u, float v) const;

private:
	point_t		m_origin;
	point_t		m_lower_left;
	vector_t	m_vec_horizontal;
	vector_t	m_vec_vertical;
	vector_t	m_u, m_v, m_w;
	float		m_lens_radius;
};

} // namespace rtiow
