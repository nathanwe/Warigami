#include <rendering/skeleton_animation.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

glm::mat4 rendering::skeletal_animation::eval(animation_time time)
{
    if (time > duration)
        time = time - duration;

	std::uint32_t pos_ind;
	while (time < position.times[pos_ind])
		pos_ind++;

	std::uint32_t rotation_ind;
	while (time < rotation.times[rotation_ind])
		rotation_ind++;

	std::uint32_t scale_ind;
	while (time < scale.times[scale_ind])
		scale_ind++;
	
	glm::mat4 t = glm::translate(glm::mat4(1.f), position.values[pos_ind]);
	glm::mat4 r = glm::mat4_cast(rotation.values[rotation_ind]);
	glm::mat4 s = glm::scale(glm::mat4(1.f), scale.values[scale_ind]);
	return t * r * s;
}
