#version 450 core

layout(early_fragment_tests) in;

in Vs_Out
{
	layout(location = 0) vec3 tex_coord;
} vs_out;

layout(binding = 0) uniform samplerCube u_cube_map;

out vec3 fs_out;

vec3 linear_from_srgb(vec3 color)
{
	const float gamma = 2.2;
	return pow(color, vec3(gamma));
}

vec3 gamma_correct(vec3 color)
{
	const float gamma = 2.2;
	return pow(color, vec3(1.0 / gamma));
}

void main()
{
	vec3 texture_color = texture(u_cube_map, vs_out.tex_coord).rgb;
	fs_out = gamma_correct(texture_color);
}