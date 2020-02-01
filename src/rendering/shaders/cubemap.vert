#version 450 core

layout(location = 0) in vec3 vs_in_position_world;
layout(location = 1) in vec2 vs_in_tex_coord;

layout(location = 0) uniform mat4 u_clip_from_world;

out Vs_Out
{
	layout(location = 0) vec3 tex_coord;
} vs_out;

void main()
{
	vs_out.tex_coord = vs_in_position_world;

	// Override position.z to 1, to always be at the back of depth tests
	gl_Position = (u_clip_from_world * vec4(vs_in_position_world, 1.0)).xyww;
}