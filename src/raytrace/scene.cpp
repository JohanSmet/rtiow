// raytrace/scene.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "scene.h"

namespace rtiow {

material_t Scene::material_create(const color_t  &albedo) {
	auto result = m_materials.size();
	m_materials.emplace_back(albedo);
	return result;
}

void Scene::sphere_add(const point_t &center, float radius, material_t material) {
	m_spheres.add_sphere(center, radius, material);
}

} // namespace rtiow

