// raytrace/geometry_base.h - Johan Smet - BSD-3-Clause (see LICENSE)

#pragma once

#include "types.h"
#include "ray.h"

namespace rtiow {

struct HitRecord {
	glm::vec3	m_point;
	glm::vec3	m_normal;
	float		m_at_t;
	bool		m_front_face;

	HitRecord() : m_at_t(std::numeric_limits<float>::infinity()) {
	}

	inline void set_face_normal(const Ray &ray, const vector_t &outward_normal) {
		m_front_face = glm::dot(ray.direction(), outward_normal) < 0;
		m_normal = m_front_face ? outward_normal : -outward_normal;
	}
};

class GeometryBase {
public:
	// hit detection
	virtual bool hit(const Ray &ray, float t_min, HitRecord &hit_record) const = 0;
};

} // namespace rtiow
