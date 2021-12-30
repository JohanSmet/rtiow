// raytrace/scene.h - Johan Smet - BSD-3-Clause (see LICENSE)
#pragma once

#include "types.h"

namespace rtiow {

struct RayTracerConfig {
	uint32_t	m_render_resolution_x = 1280;
	uint32_t	m_render_resolution_y = 720;

	uint32_t	m_samples_per_pixel = 64;
	int32_t		m_max_ray_bounces = 32;
};


} // namespace rtiow
