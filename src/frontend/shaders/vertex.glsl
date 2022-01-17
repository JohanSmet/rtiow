#version 450
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_tex_coord;

out vec2 vs_tex_coord;

void main() {
	gl_Position = vec4(in_position, 1.0f);
	vs_tex_coord = in_tex_coord;
}
