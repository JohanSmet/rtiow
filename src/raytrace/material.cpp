// raytrace/material.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "material.h"
#include "geometry_base.h"
#include "glm/geometric.hpp"
#include "scene.h"
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

color_t Material::color_at_hit(const Scene &scene, [[maybe_unused]] const Ray &ray_in, const HitRecord &hit, int32_t bounce_depth) const {

	color_t result_color(0.0f, 0.0f, 0.0f);

	// diffuse reflection
	if (m_metalness < 1.0f) {
		auto scatter_direction = hit.m_normal + random_unit_vector();
		if (glm::all(glm::epsilonEqual(scatter_direction, vector_t(0.0f, 0.0f, 0.0f), 1e-6f))) {
			scatter_direction = hit.m_normal;
		}

		result_color += (1.0f - m_metalness) * scene.ray_color(Ray(hit.m_point, scatter_direction), bounce_depth - 1);
	}

	// specular reflection
	if (m_metalness > 0.0f) {
		auto fuzz = 1.0f - m_metalness;
		auto reflected_dir = glm::reflect(glm::normalize(ray_in.direction()), hit.m_normal);
		auto reflected_ray = Ray(hit.m_point, reflected_dir + fuzz * random_vector_in_unit_sphere());

		if (glm::dot(reflected_ray.direction(), hit.m_normal) > 0) {
			result_color += m_metalness * scene.ray_color(reflected_ray, bounce_depth - 1);
		}
	}

	return m_albedo * result_color;
}


} // namespace rtiow
