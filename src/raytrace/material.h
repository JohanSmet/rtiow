// raytrace/material.h - Johan Smet - BSD-3-Clause (see LICENSE)
#pragma once

#include "ray.h"

namespace rtiow {

struct HitRecord;
class Scene;

class Material {
public:
	// construction
	Material(const color_t &albedo, float metalness):
		m_albedo(albedo), m_metalness(metalness) {
	}

	// scattering
	bool scatter(const Ray &ray_in, const HitRecord &hit, color_t &attenuation, Ray &scattered) const;

	color_t color_at_hit(const Scene &scene, const Ray &ray_in, const HitRecord &hit, int32_t bounce_depth) const;

private:
	color_t		m_albedo;
	float		m_metalness;
};

} // namespace rtiow
