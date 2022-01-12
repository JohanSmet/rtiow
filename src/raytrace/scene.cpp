// raytrace/scene.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "scene.h"

namespace rtiow {

Material &Scene::material_create_default() {
	auto &mat = m_materials.emplace_back();
	mat.m_albedo = {0.0f, 0.0f, 0.0f};
	mat.m_specular_chance =	0.0f;
	mat.m_specular_color = {0.0f, 0.0f, 0.0f};
	mat.m_specular_roughness = 0.0f;
	mat.m_index_of_refraction = 1.0f;
	mat.m_refraction_chance = 0.0f;
	mat.m_refraction_color = {0.0f, 0.0f, 0.0f};
	mat.m_refraction_roughness = 0.0f;
	return mat;
}

material_id_t Scene::material_create(
		const color_t &albedo,
		float specular_chance, const color_t &specular_color, float specular_roughness,
		float index_of_refraction,
		float refraction_chance, const color_t &refraction_color, float refraction_roughness) {

	auto &mat = material_create_default();
	mat.m_albedo = albedo;
	mat.m_specular_chance = specular_chance;
	mat.m_specular_color = specular_color;
	mat.m_specular_roughness = specular_roughness;
	mat.m_index_of_refraction = index_of_refraction;
	mat.m_refraction_chance = refraction_chance;
	mat.m_refraction_color = refraction_color;
	mat.m_refraction_roughness = refraction_roughness;

	return m_materials.size() - 1;
}

material_id_t Scene::material_create_diffuse(const color_t &albedo) {
	auto &mat = material_create_default();
	mat.m_albedo = albedo;
	return m_materials.size() - 1;
}

material_id_t Scene::material_create_specular(	const color_t &albedo,
												float specular_chance, const color_t &specular_color, float specular_roughness) {
	auto &mat = material_create_default();
	mat.m_albedo = albedo;
	mat.m_specular_chance = specular_chance;
	mat.m_specular_color = specular_color;
	mat.m_specular_roughness = specular_roughness;

	return m_materials.size() - 1;
}

material_id_t Scene::material_create_dielectric(
		const color_t &albedo, float index_of_refraction,
		float refraction_chance, const color_t &refraction_color, float refraction_roughness) {
	return material_create(albedo,
							0.0f, {0.0f, 0.0f, 0.0f}, 0.0f,
							index_of_refraction,
							refraction_chance, refraction_color, refraction_roughness);
}

void Scene::sphere_add(const point_t &center, float radius, material_id_t material) {
	m_spheres.add_sphere(center, radius, material);
}

bool Scene::hit_detection(const Ray &ray, HitRecord &hit) const {
	// there are only spheres right now
	return m_spheres.hit(ray, 0.001f, hit);
}

} // namespace rtiow

