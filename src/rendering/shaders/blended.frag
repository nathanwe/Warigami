#version 450 core

// Input ---------------------------------------------------------------------------------------
in Vs_Out
{
	layout(location = 0) vec3 position_world;
	layout(location = 1) vec3 normal_world;
	layout(location = 2) vec2 tex_coord;
	layout(location = 3) mat3 world_from_tangent;
} vs_out;

layout(binding = 0) uniform sampler2D u_tex_diffuse;
layout(binding = 1) uniform sampler2D u_tex_metalness;
layout(binding = 2) uniform sampler2D u_tex_normal;
layout(binding = 3) uniform sampler2D u_tex_roughness;
layout(binding = 4) uniform sampler2D u_tex_ambient_occlusion;

layout(location = 8) uniform vec2 u_tex_scale;
layout(location = 9) uniform vec2 u_tex_offset;

layout(location = 10) uniform bool u_has_tex_diffuse;
layout(location = 11) uniform bool u_has_tex_metalness;
layout(location = 12) uniform bool u_has_tex_normal;
layout(location = 13) uniform bool u_has_tex_roughness;
layout(location = 14) uniform bool u_has_tex_ambient_occlusion;

layout(location = 15) uniform vec3  u_param_diffuse;
layout(location = 16) uniform float u_param_metalness;
layout(location = 17) uniform float u_param_roughness;

layout(location = 35) uniform vec3 u_tint_color;

layout(location = 18) uniform vec3 u_camera_position_world; 

layout(location = 19) uniform vec3  u_light_directional_inverse_direction_world;
layout(location = 20) uniform float u_light_directional_intensity;
layout(location = 21) uniform vec3  u_light_directional_color;

#define NUM_LIGHT_POINTS 4
layout(location = 22) uniform vec3 u_light_point_position_world[NUM_LIGHT_POINTS];
layout(location = 26) uniform float u_light_point_intensity[NUM_LIGHT_POINTS];
layout(location = 30) uniform vec3 u_light_point_color[NUM_LIGHT_POINTS];
layout(location = 34) uniform int u_light_point_count;

layout(location = 36) uniform vec3 u_ambient_light_color;
layout(location = 37) uniform float u_ambient_light_intensity;
layout(location = 38) uniform bool u_has_dir_light;

// Output ---------------------------------------------------------------------------------------
layout(location = 0) out vec4 fs_out;

// Functions ---------------------------------------------------------------------------------------
vec3 normal_from_tex(sampler2D tex, vec2 uv, mat3 TBN)
{
	return normalize(TBN * normalize(texture(tex, uv).rgb * 2.0 - 1.0));
}
vec3 light_direct_diffuse_lambert(float n_dot_l, vec3 diffuse, vec3 light_color)
{
	const float pi = 3.14159265;
	return n_dot_l * diffuse / pi * light_color;
}
vec3 light_direct_specular_phong(float v_dot_r, vec3 specular, float roughness, vec3 light_color)
{
	return pow(v_dot_r, 1.0 - roughness) * specular * light_color;
}
vec3 light_direct_specular_blinn_phong(float n_dot_h, vec3 specular, float roughness, vec3 light_color)
{
	return light_direct_specular_phong(n_dot_h, specular, roughness, light_color);
}
vec3 light_direct_specular_blinn_phong_conserved(float n_dot_h, vec3 specular, float roughness, vec3 light_color)
{
	const float pi = 3.14159265;
	return ((8.0 + (1.0 - roughness)) / (8.0 + pi)) * light_direct_specular_blinn_phong(n_dot_h, specular, roughness, light_color);
}
vec3 light_attenuation(vec3 light, float distance)
{
	return light * (1.0 / (distance * distance));
}

// ---------------------------------------------------------------------------------------
void main()
{
	const float gamma = 2.2f;
	vec2 uv = vs_out.tex_coord * u_tex_scale + u_tex_offset;

	// Material
	vec3 n          = u_has_tex_normal            ? normal_from_tex(u_tex_normal, uv, vs_out.world_from_tangent) : vs_out.normal_world;
  	vec3 diffuse    = u_has_tex_diffuse           ? texture(u_tex_diffuse, uv).rgb                               : u_param_diffuse;
	//vec3 diffuse    = u_has_tex_diffuse           ? pow(texture(u_tex_diffuse, uv).rgb, vec3(gamma))             : u_param_diffuse;
	float metalness = u_has_tex_metalness         ? texture(u_tex_metalness, uv).r                               : u_param_metalness;
	float roughness = u_has_tex_roughness         ? texture(u_tex_roughness, uv).r                               : u_param_roughness;
	float ao        = u_has_tex_ambient_occlusion ? texture(u_tex_ambient_occlusion, uv).r                       : 1.0f;

	vec3 specular = mix(vec3(0.04f), diffuse, metalness);

	// Light ambient
	vec3 reflected_indirect = diffuse * ao * u_ambient_light_color * u_ambient_light_intensity;

	// Light directional
	vec3 v        = normalize(u_camera_position_world - vs_out.position_world);
	vec3 l        = normalize(u_light_directional_inverse_direction_world);
	vec3 h        = normalize(v + l);
	float n_dot_l = max(dot(n, l), 0.0);
	float n_dot_h = max(dot(n, h), 0.0);

	vec3 reflected_diffuse  = vec3(0.0f);
	vec3 reflected_specular = vec3(0.0f);
	vec3 reflected_direct   = vec3(0.0f);
	if (u_has_dir_light)
	{
		reflected_diffuse  = light_direct_diffuse_lambert(n_dot_l, diffuse, u_light_directional_color);
		reflected_specular = light_direct_specular_blinn_phong_conserved(n_dot_h, specular, roughness, u_light_directional_color);
		reflected_direct   = (reflected_diffuse + reflected_specular) * u_light_directional_intensity; // * light_directional_shadow;
	}
	vec3 pixel_color = reflected_direct + reflected_indirect;

	// Light points
	for (int i = 0; i < min(u_light_point_count, NUM_LIGHT_POINTS); ++i)
	{
		vec3 to_light = u_light_point_position_world[i] - vs_out.position_world;

		l       = normalize(to_light);
		h       = normalize(v + l);
		n_dot_l = max(dot(n, l), 0.0);
		n_dot_h = max(dot(n, h), 0.0);

		reflected_diffuse  = light_direct_diffuse_lambert(n_dot_l, diffuse, u_light_point_color[i]);
		reflected_specular = light_direct_specular_blinn_phong_conserved(n_dot_h, specular, roughness, u_light_point_color[i]);
		reflected_direct   = (reflected_diffuse + reflected_specular) * u_light_point_intensity[i];

		vec3 attenuated_reflected_direct = light_attenuation(reflected_direct, length(to_light));

		pixel_color += attenuated_reflected_direct;
		/*
		if (length(to_light) < u_light_point_radius[i])
		{
			pixel_color += vec3(1, 0, 0);
		}
		*/
	}

	vec3 rgb = pixel_color * u_tint_color;

	float a = texture(u_tex_diffuse, uv).a;

	fs_out = vec4(rgb, a);
}

#undef NUM_LIGHT_POINTS
