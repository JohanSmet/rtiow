// raytrace/raytrace.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "raytrace.h"

#include "camera.h"
#include "geometry_spheres.h"
#include "ray.h"
#include "utils.h"
#include "scene.h"

#include <cassert>

namespace rtiow {

namespace {

static inline color_t ray_color(const Ray &ray, const Scene &scene, int32_t depth) {

	// don't exceed ray bounce limit
	if (depth <= 0) {
		return color_t(0.0f, 0.0f, 0.0f);
	}

	HitRecord hit;

	if (scene.spheres().hit(ray, 0.001f, hit)) {
		auto material = scene.material(hit.m_material);
		Ray scattered;
		color_t attenuation;

		if (material.scatter(ray, hit, attenuation, scattered)) {
			return attenuation * ray_color(scattered, scene, depth - 1);
		} else {
			return color_t(0.0f, 0.0f, 0.0f);
		}
	}

	auto unit_direction = glm::normalize(ray.direction());
	auto t = 0.5f * (unit_direction.y + 1.0f);
	return (1.0f-t) * color_t(1.0f, 1.0f, 1.0f) + t * color_t(0.5f, 0.7f, 1.0f);
}

} // private namespace

RayTracer::RayTracer(const RayTracerConfig &config) : m_config(config) {
	assert(m_config.m_render_resolution_x > 0);
	assert(m_config.m_render_resolution_y > 0);
	m_output = std::make_unique<RGBBuffer>(m_config.m_render_resolution_x, m_config.m_render_resolution_y);
}

void RayTracer::render(const Scene &scene) {

	uint8_t *out = m_output->data();

	// camera
	Camera camera(m_output->width(), m_output->height());

	for (uint32_t row = 0; row < m_output->height(); ++row) {
		for (uint32_t col = 0; col < m_output->width(); ++col) {

			color_t pixel_color(0.0f, 0.0f, 0.0f);

			for (uint32_t sample = 0; sample < m_config.m_samples_per_pixel; ++sample) {

				auto u = (static_cast<float>(col) + random_float()) / static_cast<float>(m_output->width() - 1);
				auto v = (static_cast<float>(row) + random_float()) / static_cast<float>(m_output->height() - 1);

				Ray ray = camera.create_ray(u, v);
				pixel_color += ray_color(ray, scene, m_config.m_max_ray_bounces);
			}
			write_color(&out, pixel_color, m_config.m_samples_per_pixel);
		}
	}
}

} // namespace rtiow
