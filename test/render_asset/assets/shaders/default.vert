#version 460 core

layout(location = 0) in vec3 vs_in_position_local;
layout(location = 1) in vec3 vs_in_normal_local;
layout(location = 2) in vec2 vs_in_tex_coord;
layout(location = 3) in vec3 vs_in_tangent_local;
layout(location = 4) in vec3 vs_in_bitangent_local;

layout(location = 0) uniform mat4 u_world_from_local;
layout(location = 4) uniform mat4 u_clip_from_world;

out Vs_Out
{
	layout(location = 0) vec3 position_world;
	layout(location = 1) vec3 normal_world;
	layout(location = 2) vec2 tex_coord;
	layout(location = 3) mat3 world_from_tangent;
} vs_out;

mat3 make_world_from_tangent(mat4 transpose_inverse_transform, vec3 t, vec3 b, vec3 n)
{
	vec3 T = normalize(vec3(transpose_inverse_transform * vec4(t, 0)));
	vec3 B = normalize(vec3(transpose_inverse_transform * vec4(b, 0)));
	vec3 N = normalize(vec3(transpose_inverse_transform * vec4(n, 0)));
	return mat3(T, B, N);
}

void main()
{
	vec4 position_world = u_world_from_local * vec4(vs_in_position_local, 1);
	mat4 transpose_inverse_world = transpose(inverse(u_world_from_local));

	vs_out.position_world     = position_world.xyz;
	vs_out.normal_world       = normalize(vec3(transpose_inverse_world * vec4(vs_in_normal_local, 0)));
	vs_out.tex_coord          = vs_in_tex_coord;
	vs_out.world_from_tangent = make_world_from_tangent(transpose_inverse_world, vs_in_tangent_local, vs_in_bitangent_local, vs_in_normal_local);

	gl_Position = u_clip_from_world * position_world;
}
