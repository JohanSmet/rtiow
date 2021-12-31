// raytrace/scene.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "scene.h"

namespace rtiow {

material_t Scene::material_create(const color_t &albedo, float metalness) {
	auto result = m_materials.size();
	m_materials.emplace_back(albedo, metalness);
	return result;
}

void Scene::sphere_add(const point_t &center, float radius, material_t material) {
	m_spheres.add_sphere(center, radius, material);
}

void Scene::setup_rendering(const RayTracerConfig &config) {
	for (auto &mat : m_materials) {
		mat.setup_rendering(config);
	}
}

color_t Scene::ray_color(const Ray &ray, uint32_t sample_sequence, int32_t bounce_depth) const {

	// don't exceed ray bounce limit
	if (bounce_depth <= 0) {
		return color_t(0.0f, 0.0f, 0.0f);
	}

	HitRecord hit;

	if (m_spheres.hit(ray, 0.001f, hit)) {
		return material(hit.m_material).color_at_hit(*this, ray, hit, sample_sequence, bounce_depth - 1);
	}

	auto unit_direction = glm::normalize(ray.direction());
	auto t = 0.5f * (unit_direction.y + 1.0f);
	return (1.0f-t) * color_t(1.0f, 1.0f, 1.0f) + t * color_t(0.5f, 0.7f, 1.0f);
}

} // namespace rtiow

