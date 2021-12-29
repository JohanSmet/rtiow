// raytrace/ray.h - Johan Smet - BSD-3-Clause (see LICENSE)
#pragma once

#include "types.h"

namespace rtiow {

class Ray {
public:
	// construction
	Ray() = default;
	Ray(const point_t &origin, const vector_t &direction) :
		m_origin(origin),
		m_direction(direction) {
	}

	// data access
	point_t origin() const {return m_origin;}
	vector_t direction() const {return m_direction;}

	// interpolaton
	point_t at(float t) const {
		return m_origin + t * m_direction;
	}

private:
	point_t		m_origin;
	vector_t	m_direction;
};

} // namespace rtiow
