// raytrace/material.h - Johan Smet - BSD-3-Clause (see LICENSE)
#pragma once

#include "ray.h"

namespace rtiow {

class Material {
public:
	// construction
	Material(const color_t &albedo, float metalness):
		m_albedo(albedo), m_metalness(metalness) {
	}

	// rendering
	void    setup_rendering(const struct RayTracerConfig &config);
	color_t color_at_hit(const class Scene &scene,
						 const Ray &ray_in, const struct HitRecord &hit,
						 uint32_t sample_sequence, int32_t bounce_depth) const;

private:
	color_t		m_albedo;
	float		m_metalness;

	uint32_t	m_specular_samples;
};

} // namespace rtiow
