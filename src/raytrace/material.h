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

	bool scatter(const Ray &ray_in, const struct HitRecord &hit, color_t &attenuation, Ray &scattered_ray) const;

private:
	color_t		m_albedo;
	float		m_metalness;
};

} // namespace rtiow
