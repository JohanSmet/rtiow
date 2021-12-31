// raytrace/material.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "material.h"

#include "config.h"
#include "geometry_base.h"
#include "scene.h"
#include "utils.h"

namespace rtiow {

void Material::setup_rendering(const RayTracerConfig &config) {
	m_specular_samples = uint32_t(float(config.m_samples_per_pixel) * m_metalness);
}

color_t Material::color_at_hit(	const Scene &scene,
								const Ray &ray_in, const HitRecord &hit,
								uint32_t sample_sequence, int32_t bounce_depth) const {

	color_t attenuation(0.0f, 0.0f, 0.0f);

	// diffuse reflection
	if (sample_sequence > m_specular_samples) {
		auto scatter_direction = hit.m_normal + random_unit_vector();
		if (glm::all(glm::epsilonEqual(scatter_direction, vector_t(0.0f, 0.0f, 0.0f), 1e-6f))) {
			scatter_direction = hit.m_normal;
		}

		attenuation += scene.ray_color(Ray(hit.m_point, scatter_direction), sample_sequence, bounce_depth - 1);
	}

	// specular reflection
	if (sample_sequence <= m_specular_samples) {
		auto fuzz = 1.0f - m_metalness;
		auto reflected_dir = glm::reflect(glm::normalize(ray_in.direction()), hit.m_normal);
		auto reflected_ray = Ray(hit.m_point, reflected_dir + fuzz * random_vector_in_unit_sphere());

		if (glm::dot(reflected_ray.direction(), hit.m_normal) > 0) {
			attenuation += scene.ray_color(reflected_ray, sample_sequence, bounce_depth - 1);
		}
	}

	return m_albedo * attenuation;
}


} // namespace rtiow
