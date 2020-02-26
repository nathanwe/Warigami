#version 450 core

const int MAX_BONES = 128;
const float EPS = 0.00001;


layout(location = 0) in vec3 vs_in_position_local;
layout(location = 1) in vec3 vs_in_normal_local;
layout(location = 2) in vec2 vs_in_tex_coord;
layout(location = 3) in vec3 vs_in_tangent_local;
layout(location = 4) in vec3 vs_in_bitangent_local;
layout(location = 5) in ivec4 vs_bone_ids;
layout(location = 6) in vec4 vs_weights;

layout(location = 0) uniform mat4 u_world_from_local;
layout(location = 4) uniform mat4 u_clip_from_world;
layout(location = 45) uniform mat4 u_bones[MAX_BONES];

out Vs_Out
{
	layout(location = 0) vec3 position_world;
	layout(location = 1) vec3 normal_world;
	layout(location = 2) vec2 tex_coord;
	layout(location = 3) mat3 world_from_tangent;
	layout(location = 8) vec3 debug_color;
	layout(location = 9) mat4 bone_mat;
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
	mat4 bone_matrix =
		u_bones[vs_bone_ids.x] * vs_weights.x +
		u_bones[vs_bone_ids.y] * vs_weights.y +
		u_bones[vs_bone_ids.z] * vs_weights.z +
		u_bones[vs_bone_ids.w] * vs_weights.w;
	
	vec4 position_local = bone_matrix * vec4(vs_in_position_local, 1);
	vec4 position_world = u_world_from_local * position_local;
	//vec4 position_world = u_world_from_local * vec4(vs_in_position_local, 1);
	mat4 transpose_inverse_world = transpose(inverse(u_world_from_local));

	vs_out.position_world		= position_world.xyz;
	vs_out.normal_world			= normalize(vec3(transpose_inverse_world * vec4(vs_in_normal_local, 0)));
	vs_out.tex_coord			= vs_in_tex_coord;
	vs_out.world_from_tangent	= make_world_from_tangent(transpose_inverse_world, vs_in_tangent_local, vs_in_bitangent_local, vs_in_normal_local);

	vs_out.debug_color			= vec3(float(vs_bone_ids.x),  float(vs_bone_ids.y), float(vs_bone_ids.z));
	vs_out.bone_mat				= bone_matrix;

	gl_Position = u_clip_from_world * position_world;
}
