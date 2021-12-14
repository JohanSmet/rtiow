// raytrace/raytrace.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "raytrace.h"

#include <cassert>

namespace rtiow {

RayTracer::RayTracer(uint32_t output_width, uint32_t output_height) {
	assert(output_width > 0);
	assert(output_height > 0);
	m_output = std::make_unique<RGBBuffer>(output_width, output_height);
}

void RayTracer::render() {

	uint8_t *out = m_output->data();

	float conv_r = 255.999f / (static_cast<float>(m_output->width()) - 1.0f);
	float conv_g = 255.999f / (static_cast<float>(m_output->height()) - 1.0f);

	for (uint32_t row = 0; row < m_output->height(); ++row) {
		for (uint32_t col = 0; col < m_output->width(); ++col) {
			auto r = static_cast<float>(col) * conv_r;
			auto g = static_cast<float>(row) * conv_g;

			*out++ = static_cast<uint8_t> (r);
			*out++ = static_cast<uint8_t> (g);
			*out++ = 64;
		}
	}
}

} // namespace rtiow
