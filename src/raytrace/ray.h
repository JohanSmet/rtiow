// raytrace/ray.h - Johan Smet - BSD-3-Clause (see LICENSE)

#include "glm.h"

namespace rtiow {

class Ray {
public:
	// construction
	Ray(const glm::vec3 &origin, const glm::vec3 &direction) :
		m_origin(origin),
		m_direction(direction) {
	}

	// data access
	glm::vec3 origin() const {return m_origin;}
	glm::vec3 direction() const {return m_direction;}

	// interpolaton
	glm::vec3 at(float t) const {
		return m_origin + t * m_direction;
	}

private:
	glm::vec3	m_origin;
	glm::vec3	m_direction;
};

} // namespace rtiow
