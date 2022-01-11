// raytrace/material.h - Johan Smet - BSD-3-Clause (see LICENSE)
#pragma once

#include "ray.h"

namespace rtiow {

class Material {
public:
	// construction
	Material(const color_t &albedo, float specular_chance = 0.0f, color_t specular_color = {0,0,0}, float specular_roughness = 0.0f):
		m_albedo(albedo),
		m_specular_chance(specular_chance),
		m_specular_color(specular_color),
		m_specular_roughness(specular_roughness) {
	}

	bool scatter(const Ray &ray_in, const struct HitRecord &hit, color_t &attenuation, Ray &scattered_ray) const;

private:
	color_t		m_albedo;
	float		m_specular_chance;			// between 0.0f and 1.0f - chance that the reflection will choose the specular path instead of the diffuse
	color_t		m_specular_color;			// color of specular reflection
	float		m_specular_roughness;		// between 0.0f and 1.0f - how blurry the specular reflection is (0.0 == sharp, 1.0 == blurry)
};

} // namespace rtiow
