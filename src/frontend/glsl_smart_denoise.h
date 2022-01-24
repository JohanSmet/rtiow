// frontend/glsl_smart_denoise.h - Johan Smet - BSD-3-Clause (see LICENSE)
#pragma once

#include <raytrace/types.h>
#include "opengl_uniform_buffer.h"

namespace rtiow {

namespace gui {

class FilterGlslSmartDeNoise {
public:
	// construction
	FilterGlslSmartDeNoise(class OutputOpenGL *output);
	FilterGlslSmartDeNoise(const FilterGlslSmartDeNoise &) = delete;
	FilterGlslSmartDeNoise(const FilterGlslSmartDeNoise &&) = delete;
	FilterGlslSmartDeNoise &operator=(const FilterGlslSmartDeNoise &) = delete;
	~FilterGlslSmartDeNoise();

	void set_image_size(int32_t width, int32_t height);
	void imgui_config_window();

private:
	struct params_t {
		float		uSigma;
		float		uThreshold;
		float		uKSigma;
		float		padding;
		glm::vec2	wSize;
	};

private:
	class OutputOpenGL *	m_output;
	bool					m_enabled;
	params_t				m_parameters;
	OpenGLUniformBuffer		m_ubo_parameters;

	std::unique_ptr<class OpenGLShader>		m_shader;
};

} // namespace rtiow::gui

} // namespace rtiow
