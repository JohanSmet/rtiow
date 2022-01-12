// raytrace/scene.h - Johan Smet - BSD-3-Clause (see LICENSE)
#pragma once

#include "geometry_spheres.h"

#include <vector>

namespace rtiow {

struct Material {
	color_t		m_albedo;

	float		m_specular_chance;			// chance that the reflection will choose the specular path instead of the diffuse (0 .. 1)
	color_t		m_specular_color;			// color of specular reflection
	float		m_specular_roughness;		// how blurry the specular reflection is (from 0.0 == sharp to 1.0 == blurry)

	float		m_index_of_refraction;
	float		m_refraction_chance;		// change that the reflection will choose the refraction path (0 .. 1 - specular-change)
	color_t		m_refraction_color;			// color of the refraction
	float		m_refraction_roughness;		// how blurry the refraction is (from 0.0 == sharp to 1.0 == blurry)
};

class Scene {
public:
	// construction
	Scene() = default;

	// material
	material_id_t material_create(const color_t &albedo,
							   float specular_chance, const color_t &specular_color, float specular_roughness,
							   float index_of_refraction,
							   float refraction_chance, const color_t &refraction_color, float refraction_roughness);

	material_id_t material_create_diffuse(const color_t &albedo);
	material_id_t material_create_specular(const color_t &albedo, float specular_chance, const color_t &specular_color, float specular_roughness);
	material_id_t material_create_dielectric(const color_t &albedo, float index_of_refraction,
										  float refraction_chance, const color_t &refraction_color, float refraction_roughness);

	const Material &material(material_id_t material) const {
		assert(material < m_materials.size());
		return m_materials[material];
	}

	// geometry
	const GeometrySpheres &spheres() const {return m_spheres;}
	void sphere_add(const point_t &center, float radius, material_id_t material);

	// ray tracing
	bool hit_detection(const Ray &ray, HitRecord &hit) const;

private:
	Material &material_create_default();

private:
	GeometrySpheres			m_spheres;
	std::vector<Material>	m_materials;
};

} // namespace rtiow
