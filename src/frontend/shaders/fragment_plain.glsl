#version 450
in vec2 vs_tex_coord;
layout (binding = 0) uniform sampler2D tex;

layout (location = 0) out vec4 out_color;

void main() {
	out_color = texture(tex, vs_tex_coord);
}
