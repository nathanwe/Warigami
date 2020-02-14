#version 450 core

layout(location = 0) uniform vec3 u_color;

layout(location = 4) out vec3 fs_out;

void main()
{
	fs_out = u_color;
}
