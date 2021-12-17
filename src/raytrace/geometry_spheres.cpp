// raytrace/geometry_base.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "geometry_spheres.h"

namespace rtiow {

namespace {

static inline bool hit_sphere(const Sphere &sphere, const Ray &ray, float t_min, float t_max, HitRecord &hit_record) {

	auto oc = ray.origin() - sphere.m_center;

	auto a = glm::dot(ray.direction(), ray.direction());
	auto half_b = glm::dot(oc, ray.direction());
	auto c = glm::dot(oc, oc) - (sphere.m_radius * sphere.m_radius);

	auto discriminant = (half_b * half_b) - (a * c);
	if (discriminant < 0) {
		// negative discriminat => no roots => no collision
		return false;				// early exit !!!
	}

	auto sqrt_discriminant = sqrtf(discriminant);

	// find the nearest root in the target range
	auto root = (-half_b - sqrt_discriminant) / a;
	if (root < t_min || root > t_max) {
		root = (-half_b + sqrt_discriminant) / a;
		if (root < t_min || root > t_max) {
			// collision but outside of acceptable range
			return false;			// early exit !!!
		}
	}

	// register hit
	hit_record.m_at_t  = root;
	hit_record.m_point = ray.at(root);
	hit_record.set_face_normal(ray, (hit_record.m_point - sphere.m_center) / sphere.m_radius);

	return true;
}


} // unnamed namespace

void GeometrySpheres::clear() {
	m_spheres.clear();
}

void GeometrySpheres::add_sphere(const point_t &center, float radius) {
	m_spheres.push_back(Sphere{center, radius});
}

bool GeometrySpheres::hit(const Ray &ray, float t_min, HitRecord &hit_record) const {

	bool hit_anything = false;

	for (const auto &sphere: m_spheres) {
		if (hit_sphere(sphere, ray, t_min, hit_record.m_at_t, hit_record)) {
			hit_anything = true;
		}
	}

	return hit_anything;
}


} // namespace rtiow
