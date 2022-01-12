// raytrace/geometry_spheres.h - Johan Smet - BSD-3-Clause (see LICENSE)
#pragma once

#include "geometry_base.h"
#include <vector>

namespace rtiow {

struct Sphere {
	point_t		m_center;
	float		m_radius;
	material_id_t	m_material;
};

class GeometrySpheres: public GeometryBase {
public:
	// construction
	GeometrySpheres() = default;
	GeometrySpheres(const GeometrySpheres &other) = delete;
	GeometrySpheres(const GeometrySpheres &&other) = delete;
	virtual ~GeometrySpheres() = default ;

	// object interface
	void clear();
	void add_sphere(const point_t &center, float radius, material_id_t material);

	// hit detection
	bool hit(const Ray &ray, float t_min, HitRecord &hit_record) const;

private:
	std::vector<Sphere>		m_spheres;
};


} // namespace rtiow
