// frontend/glsl_smart_denoise.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "glsl_smart_denoise.h"

#include "raytrace/glm.h"
#include "output_opengl.h"
#include "opengl_shader.h"

#include <imgui.h>

namespace rtiow {

namespace gui {

FilterGlslSmartDeNoise::FilterGlslSmartDeNoise(OutputOpenGL *output) :
	m_output(output),
	m_enabled(false),
	m_parameters {
		7.0f,			// sigma
		0.180f,			// threshold
		1.0f,			// KSigma
		0.0f,			// padding
		{0.0f, 0.0f}	// window size
	}
{
	m_ubo_parameters.init(sizeof(m_parameters), 1);
	m_ubo_parameters.update(m_parameters);

	m_shader = OpenGLShader::create_from_files(
						"src/frontend/shaders/vertex.glsl",
						"src/frontend/shaders/fragment_denoise.glsl");
}

FilterGlslSmartDeNoise::~FilterGlslSmartDeNoise() {
}

void FilterGlslSmartDeNoise::set_image_size(int32_t width, int32_t height) {
	m_parameters.wSize.x = float(width);
	m_parameters.wSize.y = float(height);
	m_ubo_parameters.update(m_parameters);
}

void FilterGlslSmartDeNoise::imgui_config_window() {

	ImGui::SetNextWindowSize(ImVec2(300, 0), ImGuiCond_Appearing);

	ImGui::Begin("glslSmartDenoise");
		const float w_width = ImGui::GetContentRegionAvail().x;
		bool update_ubo = false;

		if (ImGui::Checkbox("Enable", &m_enabled)) {
			m_output->set_active_shader((m_enabled) ? m_shader.get() : nullptr);
		}
		ImGui::Separator();

		const auto bar_width = ((w_width - ImGui::CalcTextSize("Radius: x").x) / 2.0f) - 6.0f;

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Radius:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(bar_width);
		update_ubo |= ImGui::SliderFloat("##dev_std", &m_parameters.uKSigma, 0.0f, 3.0f, "DevStd %.2f");
		ImGui::SameLine();

		ImGui::Text("x");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(bar_width);
		update_ubo |= ImGui::DragFloat("##sigma", &m_parameters.uSigma, 0.025f, 0.001f, 24.0f, "Sigma %.3f");

		ImGui::SetNextItemWidth(w_width);
		update_ubo |= ImGui::DragFloat("##threshold",&m_parameters.uThreshold, 0.001f, 0.001f, 2.0f, "Edge threshold %.3f");
	ImGui::End();

	if (update_ubo) {
		m_ubo_parameters.update(m_parameters);
	}
}

} // namespace rtiow::gui

} // namespace rtiow
