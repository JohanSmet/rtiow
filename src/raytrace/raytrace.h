// raytrace/raytrace.h - Johan Smet - BSD-3-Clause (see LICENSE)

#pragma once

#include <memory>

#include "rgb_buffer.h"

namespace rtiow {

class RayTracer {
public:
	// construction
	RayTracer(uint32_t output_width, uint32_t output_height);

	// data access
	const uint8_t *output_ptr() const {return m_output->data();}

	// rendering
	void render();


private:
	std::unique_ptr<RGBBuffer>	m_output;
};


}
