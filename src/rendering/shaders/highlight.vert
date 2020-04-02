#version 450 core

layout(location = 0) in vec3 vs_in_position_local;
layout(location = 1) in vec3 vs_in_normal_local;
layout(location = 2) in vec2 vs_in_tex_coord;
layout(location = 3) in vec3 vs_in_tangent_local;
layout(location = 4) in vec3 vs_in_bitangent_local;

layout(location = 0) uniform mat4 u_clip_from_local;

void main()
{
	gl_Position = u_clip_from_local * vec4(vs_in_position_local, 1);
}
