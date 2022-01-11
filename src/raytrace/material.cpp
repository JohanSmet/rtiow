// raytrace/material.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "material.h"

#include "config.h"
#include "geometry_base.h"
#include "scene.h"
#include "utils.h"

namespace rtiow {

bool Material::scatter(const Ray &ray_in, const struct HitRecord &hit, color_t &attenuation, Ray &scattered_ray) const {

	// decide how this ray will be reflected
	bool choose_specular = random_float() < m_metalness;

	// calculate the new ray's direction
	if (!choose_specular) {
		auto scatter_direction = hit.m_normal + random_unit_vector();
		if (glm::all(glm::epsilonEqual(scatter_direction, vector_t(0.0f, 0.0f, 0.0f), 1e-6f))) {
			scatter_direction = hit.m_normal;
		}
		scattered_ray = Ray(hit.m_point, scatter_direction);
		attenuation = m_albedo;
		return true;
	} else {
		auto fuzz = 1.0f - m_metalness;
		auto reflected_dir = glm::reflect(glm::normalize(ray_in.direction()), hit.m_normal);
		scattered_ray = Ray(hit.m_point, reflected_dir + fuzz * random_vector_in_unit_sphere());

		attenuation = m_albedo;

		return (glm::dot(scattered_ray.direction(), hit.m_normal) > 0);
	}
}

} // namespace rtiow
