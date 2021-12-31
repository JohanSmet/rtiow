// raytrace/raytrace.h - Johan Smet - BSD-3-Clause (see LICENSE)

#pragma once

#include <memory>
#include "config.h"
#include "rgb_buffer.h"
#include "scene.h"

namespace rtiow {

class RayTracer {
public:
	// construction
	RayTracer(const RayTracerConfig &config);

	// data access
	const uint8_t *output_ptr() const {return m_output->data();}

	// rendering
	void render(Scene &scene);

private:
	RayTracerConfig				m_config;
	std::unique_ptr<RGBBuffer>	m_output;
};

}
