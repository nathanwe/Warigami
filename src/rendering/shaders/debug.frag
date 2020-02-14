#version 450 core

layout(location = 4) uniform vec3 u_color;

out vec3 fs_out;

void main()
{
	fs_out = u_color;
}
