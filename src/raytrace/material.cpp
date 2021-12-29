// raytrace/material.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "material.h"
#include "geometry_base.h"
#include "utils.h"

namespace rtiow {

bool Material::scatter([[maybe_unused]] const Ray &ray_in, const HitRecord &hit, color_t &attenuation, Ray &scattered) const {
	// lambertian
	auto scatter_direction = hit.m_normal + random_unit_vector();
	if (glm::all(glm::epsilonEqual(scatter_direction, vector_t(0.0f, 0.0f, 0.0f), 1e-6f))) {
		scatter_direction = hit.m_normal;
	}

	scattered = Ray(hit.m_point, scatter_direction);
	attenuation = m_albedo;
	return true;
}


} // namespace rtiow
