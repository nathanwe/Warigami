#version 460 core

layout(location = 0) in vec3 vsIn_Position;

layout(location = 0) uniform mat4 u_WorldViewProjection;

void main()
{
	gl_Position = u_WorldViewProjection * vsIn_Position;
}
