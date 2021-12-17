// raytrace/raytrace.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "raytrace.h"

#include "camera.h"
#include "geometry_spheres.h"
#include "ray.h"
#include "utils.h"

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

} // private namespace

RayTracer::RayTracer(uint32_t output_width, uint32_t output_height) {
	assert(output_width > 0);
	assert(output_height > 0);
	m_output = std::make_unique<RGBBuffer>(output_width, output_height);
}

void RayTracer::render() {

	uint8_t *out = m_output->data();

	// image
	constexpr int samples_per_pixel = 32;

	// world
	GeometrySpheres world;
	world.add_sphere(point_t(0.0f, 0.0f, -1.0f), 0.5f);
	world.add_sphere(point_t(0.0f, -100.5f, -1.0f), 100.0f);

	// camera
	Camera camera(m_output->width(), m_output->height());

	for (uint32_t row = 0; row < m_output->height(); ++row) {
		for (uint32_t col = 0; col < m_output->width(); ++col) {

			color_t pixel_color(0.0f, 0.0f, 0.0f);

			for (int sample = 0; sample < samples_per_pixel; ++sample) {

				auto u = (static_cast<float>(col) + random_float()) / static_cast<float>(m_output->width() - 1);
				auto v = (static_cast<float>(row) + random_float()) / static_cast<float>(m_output->height() - 1);

				Ray ray = camera.create_ray(u, v);
				pixel_color += ray_color(ray, world);
			}
			write_color(&out, pixel_color, samples_per_pixel);
		}
	}
}

} // namespace rtiow
