// raytrace/scene.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "scene.h"

namespace rtiow {

material_t Scene::material_create_diffuse(const color_t &albedo) {
	auto result = m_materials.size();
	m_materials.emplace_back(albedo);
	return result;
}

material_t Scene::material_create_specular(	const color_t &albedo,
											float specular_chance, const color_t &specular_color, float specular_roughness) {
	auto result = m_materials.size();
	m_materials.emplace_back(albedo, specular_chance, specular_color, specular_roughness);
	return result;
}

void Scene::sphere_add(const point_t &center, float radius, material_t material) {
	m_spheres.add_sphere(center, radius, material);
}

color_t Scene::ray_color(const Ray &ray, int32_t bounce_depth) const {

	// don't exceed ray bounce limit
	if (bounce_depth <= 0) {
		return color_t(0.0f, 0.0f, 0.0f);
	}

	HitRecord hit;

	if (m_spheres.hit(ray, 0.001f, hit)) {
		color_t attenuation;
		Ray next_ray;

		if (material(hit.m_material).scatter(ray, hit, attenuation, next_ray)) {
			return attenuation * ray_color(next_ray, bounce_depth - 1);
		} else {
			return color_t(0.0f, 0.0f, 0.0f);
		}
	}

	// no hit => environment
	auto unit_direction = glm::normalize(ray.direction());
	auto t = 0.5f * (unit_direction.y + 1.0f);
	return (1.0f-t) * color_t(1.0f, 1.0f, 1.0f) + t * color_t(0.5f, 0.7f, 1.0f);
}

} // namespace rtiow

