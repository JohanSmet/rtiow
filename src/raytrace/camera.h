// raytrace/camera.h - Johan Smet - BSD-3-Clause (see LICENSE)
#pragma once

#include "ray.h"

namespace rtiow {

class Camera {
public:
	// construction
	Camera(uint32_t img_width, uint32_t img_height);

	// ray generation
	inline Ray create_ray(float u, float v) {
		return Ray(m_origin, m_lower_left + (u * m_vec_horizontal) + (v * m_vec_vertical) - m_origin);
	}

private:
	point_t		m_origin;
	point_t		m_lower_left;
	vector_t	m_vec_horizontal;
	vector_t	m_vec_vertical;
};

} // namespace rtiow
