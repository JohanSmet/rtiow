// frontend//opengl_shader.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "opengl_shader.h"

#include <glad/glad.h>
#include <cstdio>
#include <iterator>
#include <string>
#include <fstream>

namespace rtiow {

namespace gui {

namespace utils {

static inline void check_and_log_shader_error(uint32_t id, bool program, const char *source = "") {
	int success;
	char compile_log[512];

	if (!program) {
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	} else {
		glGetProgramiv(id, GL_LINK_STATUS, &success);
	}

	if(!success)
	{
		if (!program) {
			glGetShaderInfoLog(id, 512, NULL, compile_log);
		} else {
			glGetProgramInfoLog(id, 512, NULL, compile_log);
		}
		fprintf(stderr, "%s\n%s", source, compile_log);
	};
}

} // namespace utils

OpenGLShader::~OpenGLShader() {
	if (m_stage_ids[0] != INVALID) {
		glDetachShader(m_program_id, m_stage_ids[0]);
		glDeleteShader(m_stage_ids[0]);
	}

	if (m_stage_ids[1] != INVALID) {
		glDetachShader(m_program_id, m_stage_ids[1]);
		glDeleteShader(m_stage_ids[1]);
	}

	if (m_program_id != INVALID) {
		glDeleteProgram(m_program_id);
	}
}

std::unique_ptr<OpenGLShader> OpenGLShader::create_from_files(const char *vertex_filename, const char *fragment_filename) {
	auto v = compile_stage_from_file(GL_VERTEX_SHADER, vertex_filename);
	auto f = compile_stage_from_file(GL_FRAGMENT_SHADER, fragment_filename);
	auto p = link(v, f);

	return std::make_unique<OpenGLShader>(v, f, p);
}

uint32_t OpenGLShader::compile_stage_from_file(uint32_t shader_stage, const char *filename) {

	std::ifstream file(filename);
	if (!file.is_open()) {
		fprintf(stderr, "Error opening shader source (%s)\n", filename);
		exit(-1);
	}

	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return compile_stage_from_source(shader_stage, content.c_str());
}

uint32_t OpenGLShader::compile_stage_from_source(uint32_t shader_stage, const char *source) {
	auto stage_id = glCreateShader(shader_stage);
	glShaderSource(stage_id, 1, &source, nullptr);
	glCompileShader(stage_id);
	utils::check_and_log_shader_error(stage_id, false, source);
	return stage_id;
}

uint32_t OpenGLShader::link(uint32_t vertex, uint32_t fragment) {
	auto program_id = glCreateProgram();
	glAttachShader(program_id, vertex);
	glAttachShader(program_id, fragment);
	glLinkProgram(program_id);
	utils::check_and_log_shader_error(program_id, true);
	return program_id;
}

void OpenGLShader::bind() {
	glUseProgram(m_program_id);
}

} // namespace rtiow::gui

} // rtiow

