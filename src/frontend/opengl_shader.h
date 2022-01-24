// frontend/opengl_shader.h - Johan Smet - BSD-3-Clause (see LICENSE)
#pragma once

#include <raytrace/types.h>

namespace rtiow {
namespace gui {

class OpenGLShader {
public:
	OpenGLShader() = default;
	OpenGLShader(uint32_t vertex_stage, uint32_t fragment_stage, uint32_t program_id) :
		m_stage_ids{vertex_stage, fragment_stage},
		m_program_id(program_id) {
	}

	OpenGLShader(const OpenGLShader &) = delete;
	OpenGLShader(const OpenGLShader &&) = delete;
	~OpenGLShader();

	static std::unique_ptr<OpenGLShader> create_from_files(const char *vertex_filename, const char *fragment_filename);

	void bind();

private:
	static uint32_t compile_stage_from_file(uint32_t shader_stage, const char *filename);
	static uint32_t compile_stage_from_source(uint32_t shader_stage, const char *source);
	static uint32_t link(uint32_t vertex, uint32_t fragment);

private:
	static constexpr uint32_t INVALID = UINT32_MAX;
	uint32_t m_stage_ids[2] = {INVALID, INVALID};
	uint32_t m_program_id = INVALID;
};

} // namespace rtiow::gui
} // namespace rtiow

