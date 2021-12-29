// raytrace/material.h - Johan Smet - BSD-3-Clause (see LICENSE)
#pragma once

#include "ray.h"

namespace rtiow {

struct HitRecord;

class Material {
public:
	// construction
	Material(const color_t &albedo) : m_albedo(albedo) {
	}

	// scattering
	bool scatter(const Ray &ray_in, const HitRecord &hit, color_t &attenuation, Ray &scattered) const;

private:
	color_t		m_albedo;
};

} // namespace rtiow
