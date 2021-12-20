// raytrace/utils.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// utility functions

#pragma once

#include <random>
#include "glm/geometric.hpp"
#include "types.h"

namespace rtiow {

inline float random_float() {
	static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
	static std::mt19937 generator;

	return distribution(generator);
}

inline float random_float(float min, float max) {
	return min + ((max - min) * random_float());
}

inline vector_t random_vector() {
	return vector_t(random_float(), random_float(), random_float());
}

inline vector_t random_vector(float min, float max) {
	return vector_t(random_float(min, max), random_float(min, max), random_float(min, max));
}

inline vector_t random_vector_in_unit_sphere() {
	while (true) {
		auto v = random_vector(-1.0f, 1.0f);
		if (glm::dot(v, v) < 1.0f) {
			return v;
		}
	}
}

inline vector_t random_unit_vector() {
	return glm::normalize(random_vector_in_unit_sphere());
}

inline void write_color(uint8_t **out, const color_t &color, int32_t samples_per_pixel) {

	auto scale = 1.0f / static_cast<float>(samples_per_pixel);
	auto r = glm::clamp(sqrtf(color.r * scale), 0.0f, 0.999f);
	auto g = glm::clamp(sqrtf(color.g * scale), 0.0f, 0.999f);
	auto b = glm::clamp(sqrtf(color.b * scale), 0.0f, 0.999f);

	*(*out)++ = static_cast<uint8_t>(255.999f * r);
	*(*out)++ = static_cast<uint8_t>(255.999f * g);
	*(*out)++ = static_cast<uint8_t>(255.999f * b);
}


} // namespace rtiow
