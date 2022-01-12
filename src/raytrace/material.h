// raytrace/material.h - Johan Smet - BSD-3-Clause (see LICENSE)
#pragma once

#include "ray.h"

namespace rtiow {

class Material {
public:
	// construction
	Material(const color_t &albedo,
			 float specular_chance = 0.0f, color_t specular_color = {0,0,0}, float specular_roughness = 0.0f,
			 float index_of_refraction = 1.0f,
			 float refraction_chance = 0.0f, color_t refraction_color = {0,0,0}, float refraction_roughness = 0.0f):
		m_albedo(albedo),
		m_specular_chance(specular_chance),
		m_specular_color(specular_color),
		m_specular_roughness(specular_roughness),
		m_index_of_refraction(index_of_refraction),
		m_refraction_chance(refraction_chance),
		m_refraction_color(refraction_color),
		m_refraction_roughness(refraction_roughness) {
	}

	bool scatter(const Ray &ray_in, const struct HitRecord &hit, color_t &attenuation, Ray &scattered_ray) const;

private:
	color_t		m_albedo;

	float		m_specular_chance;			// chance that the reflection will choose the specular path instead of the diffuse (0 .. 1)
	color_t		m_specular_color;			// color of specular reflection
	float		m_specular_roughness;		// how blurry the specular reflection is (from 0.0 == sharp to 1.0 == blurry)

	float		m_index_of_refraction;
	float		m_refraction_chance;		// change that the reflection will choose the refraction path (0 .. 1 - specular-change)
	color_t		m_refraction_color;			// color of the refraction
	float		m_refraction_roughness;		// how blurry the refraction is (from 0.0 == sharp to 1.0 == blurry)
};

} // namespace rtiow
