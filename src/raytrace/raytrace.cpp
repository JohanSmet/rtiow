// raytrace/raytrace.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
#include "raytrace.h"
#include "ray.h"

#include <cassert>

namespace rtiow {

using Color = glm::vec3;

namespace {

static inline bool hit_sphere(const glm::vec3 &center, float radius, const Ray &ray) {
	auto oc = ray.origin() - center;

	auto a = glm::dot(ray.direction(), ray.direction());
	auto b = 2.0f * glm::dot(oc, ray.direction());
	auto c = glm::dot(oc, oc) - (radius * radius);

	auto discriminant = (b * b) - (4 * a * c);
	return discriminant > 0;
}

static inline Color ray_color([[maybe_unused]] const Ray &ray) {
	if (hit_sphere(glm::vec3(0, 0, -1), 0.5, ray)) {
		return Color(1.0f, 0.0f, 0.0f);
	}

	auto unit_direction = glm::normalize(ray.direction());
	auto t = 0.5f * (unit_direction.y + 1.0f);
	return (1.0f-t) * Color(1.0f, 1.0f, 1.0f) + t * Color(0.5f, 0.7f, 1.0f);
}

static inline void write_color(uint8_t **out, const Color &color) {
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

	// camera
	auto viewport_height = 2.0f;
	auto viewport_width = viewport_height * aspect_ratio;
	auto focal_length = 1.0f;

	auto origin = glm::vec3(0.0f);
	auto horizontal = glm::vec3(viewport_width, 0, 0);
	auto vertical = glm::vec3(0, viewport_height, 0);
	auto lower_left = origin - (horizontal / 2.0f) - (vertical / 2.0f) - glm::vec3(0, 0, focal_length);


	//float conv_r = 255.999f / (static_cast<float>(m_output->width()) - 1.0f);
	//float conv_g = 255.999f / (static_cast<float>(m_output->height()) - 1.0f);

	for (uint32_t row = 0; row < m_output->height(); ++row) {
		for (uint32_t col = 0; col < m_output->width(); ++col) {
			auto u = static_cast<float>(col) / static_cast<float>(m_output->width() - 1);
			auto v = static_cast<float>(row) / static_cast<float>(m_output->height() - 1);

			Ray ray(origin, lower_left + (u * horizontal) + (v * vertical) - origin);

			auto pixel_color = ray_color(ray);
			write_color(&out, pixel_color);
		}
	}
}

} // namespace rtiow
