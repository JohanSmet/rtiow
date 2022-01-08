// raytrace/scene.h - Johan Smet - BSD-3-Clause (see LICENSE)
#pragma once

#include "types.h"

namespace rtiow {

struct RayTracerConfig {
	uint32_t	m_render_resolution_x = 1280;		// horizontal resolution
	uint32_t	m_render_resolution_y = 720;		// vertical resolution

	uint32_t	m_samples_per_pixel = 64;			// multi-sampling: number of sample points per pixel
	int32_t		m_max_ray_bounces = 32;				// maximum number of ray bounces before giving up

	int32_t		m_threads_ignore = 1;				// number of hardware threads to ignore and leave available for other system tasks
	int32_t		m_threads_use_percent = 100;		// percentage of available hardware threads to actually use for raytracing
													//	=> render thread pool size = (system hardware threads - ignore) * use_percent / 100
	int32_t		m_num_render_workers = 0;			// manually override computed number of workers when <> 0
};


} // namespace rtiow
