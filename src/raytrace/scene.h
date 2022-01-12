// raytrace/scene.h - Johan Smet - BSD-3-Clause (see LICENSE)
#pragma once

#include "geometry_spheres.h"
#include "material.h"

#include <vector>

namespace rtiow {

class Scene {
public:
	// construction
	Scene() = default;

	// material
	material_t material_create(const color_t &albedo,
							   float specular_chance, const color_t &specular_color, float specular_roughness,
							   float index_of_refraction,
							   float refraction_chance, const color_t &refraction_color, float refraction_roughness);

	material_t material_create_diffuse(const color_t &albedo);
	material_t material_create_specular(const color_t &albedo, float specular_chance, const color_t &specular_color, float specular_roughness);
	material_t material_create_dielectric(const color_t &albedo, float index_of_refraction,
										  float refraction_chance, const color_t &refraction_color, float refraction_roughness);

	const Material &material(material_t material) const {
		assert(material < m_materials.size());
		return m_materials[material];
	}

	// geometry
	const GeometrySpheres &spheres() const {return m_spheres;}
	void sphere_add(const point_t &center, float radius, material_t material);

	// ray tracing
	color_t ray_color(const Ray &ray, int32_t bounce_depth) const;

private:
	GeometrySpheres			m_spheres;
	std::vector<Material>	m_materials;
};

} // namespace rtiow
