// raytrace/scene.h - Johan Smet - BSD-3-Clause (see LICENSE)
#pragma once

#include "geometry_spheres.h"
#include "material.h"

#include <vector>

namespace rtiow {

class Scene {
public:
	// construction
	Scene() {};

	// material
	material_t material_create(const color_t &albedo);

	const Material &material(material_t material) const {
		assert(material < m_materials.size());
		return m_materials[material];
	}

	// geometry
	const GeometrySpheres &spheres() const {return m_spheres;}
	void sphere_add(const point_t &center, float radius, material_t material);

private:
	GeometrySpheres			m_spheres;
	std::vector<Material>	m_materials;
};

} // namespace rtiow