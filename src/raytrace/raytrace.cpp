// raytrace/raytrace.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "raytrace.h"
#include "ray.h"
#include "geometry_spheres.h"

#include <cassert>

namespace rtiow {

namespace {

static inline color_t ray_color(const Ray &ray, const GeometryBase &world) {

	HitRecord hit;

	if (world.hit(ray, 0, hit)) {
		return 0.5f * (hit.m_normal + color_t(1.0f, 1.0f, 1.0f));
	}

	auto unit_direction = glm::normalize(ray.direction());
	auto t = 0.5f * (unit_direction.y + 1.0f);
	return (1.0f-t) * color_t(1.0f, 1.0f, 1.0f) + t * color_t(0.5f, 0.7f, 1.0f);
}

static inline void write_color(uint8_t **out, const color_t &color) {
	*(*out)++ = static_cast<uint8_t>(255.999f * color.r);
	*(*out)++ = static_cast<uint8_t>(255.999f * color.g);
	*(*out)++ = static_cast<uint8_t>(255.999f * color.b);
}

} // private namespace

RayTracer::RayTracer(uint32_t output_width, uint32_t output_height) {
	assert(output_width > 0);
	assert(output_height > 0);
	m_output = std::make_unique<RGBBuffer>(output_width, output_height);
}

void RayTracer::render() {

	uint8_t *out = m_output->data();

	// image
	const auto aspect_ratio = static_cast<float> (m_output->width()) / static_cast<float>(m_output->height());

	// world
	GeometrySpheres world;
	world.add_sphere(point_t(0.0f, 0.0f, -1.0f), 0.5f);
	world.add_sphere(point_t(0.0f, -100.5f, -1.0f), 100.0f);

	// camera
	auto viewport_height = 2.0f;
	auto viewport_width = viewport_height * aspect_ratio;
	auto focal_length = 1.0f;

	auto origin = point_t(0.0f);
	auto horizontal = vector_t(viewport_width, 0, 0);
	auto vertical = vector_t(0, viewport_height, 0);
	auto lower_left = origin - (horizontal / 2.0f) - (vertical / 2.0f) - vector_t(0, 0, focal_length);

	for (uint32_t row = 0; row < m_output->height(); ++row) {
		for (uint32_t col = 0; col < m_output->width(); ++col) {
			auto u = static_cast<float>(col) / static_cast<float>(m_output->width() - 1);
			auto v = static_cast<float>(row) / static_cast<float>(m_output->height() - 1);

			Ray ray(origin, lower_left + (u * horizontal) + (v * vertical) - origin);

			auto pixel_color = ray_color(ray, world);
			write_color(&out, pixel_color);
		}
	}
}

} // namespace rtiow
