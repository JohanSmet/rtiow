// raytrace/image_buffer.h - Johan Smet - BSD-3-Clause (see LICENSE)

#pragma once

#include <vector>
#include <cstdint>

namespace rtiow {

class RGBBuffer {
public:
	// construction
	RGBBuffer() = delete;
	RGBBuffer(uint32_t width, uint32_t height) : m_width(width), m_height(height) {
		m_buffer.resize(width * height * 3);
	}
	RGBBuffer(const RGBBuffer &other) = delete;

	// data access
	uint32_t width() const {return m_width;}
	uint32_t height() const {return m_height;}

	const uint8_t *data() const {return m_buffer.data();}
	uint8_t *data() {return m_buffer.data();}

private:
	uint32_t				m_width;
	uint32_t				m_height;
	std::vector<uint8_t>	m_buffer;
};

} // namespace rtiow
