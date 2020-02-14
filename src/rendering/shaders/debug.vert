#version 450 core

layout(location = 0) in vec3 vs_in_position_local;

layout(location = 0) uniform mat4 u_clip_from_local;

void main()
{
	gl_Position = u_clip_from_local * vec4(vs_in_position_local, 1);
}
