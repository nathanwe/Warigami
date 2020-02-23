#include <rendering/skeleton_animation.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>


glm::mat4 rendering::skeletal_animation::eval(animation_time time)
{
    if (duration == animation_time::zero())
    {
        return glm::mat4(1.f);
    }

	if (time == animation_time::zero())
	{
		glm::mat4 t = glm::translate(glm::mat4(1.f), position.values[0]);
		glm::mat4 r = glm::mat4_cast(rotation.values[0]);
		glm::mat4 s = glm::scale(glm::mat4(1.f), scale.values[0]);
		return t * r * s;
	}

    if (time > duration)
        time = time - duration;

	std::uint16_t pos_ind{ 0 };
	while (time < position.times[pos_ind])
		pos_ind++;

	std::uint16_t rotation_ind{ 0 };
	while (time < rotation.times[rotation_ind])
		rotation_ind++;

	std::uint16_t scale_ind{ 0 };
	while (time < scale.times[scale_ind])
		scale_ind++;	
		
	auto t_range = position.times[pos_ind] - position.times[pos_ind-1];
	float pos_interp_t = (time - position.times[pos_ind - 1]) / t_range;
	auto interpolated_pos =  glm::mix(position.values[pos_ind-1], position.values[pos_ind], pos_interp_t);
		
	t_range = rotation.times[rotation_ind] - rotation.times[rotation_ind-1];
	float rot_interp_t = (time - rotation.times[rotation_ind - 1]) / t_range;
	auto interpolated_rot = glm::slerp(rotation.values[rotation_ind-1], rotation.values[rotation_ind], rot_interp_t);
		
	t_range = scale.times[scale_ind] - scale.times[scale_ind-1];
	auto scl_interp_t = (time - scale.times[scale_ind - 1]) / t_range;
	auto interpolated_scale = glm::mix(scale.values[scale_ind - 1], scale.values[scale_ind], scl_interp_t);
	
	glm::mat4 t = glm::translate(glm::mat4(1.f), interpolated_pos);
	glm::mat4 r = glm::mat4_cast(interpolated_rot);
	glm::mat4 s = glm::scale(glm::mat4(1.f), interpolated_scale);
	return t * r * s;
}
