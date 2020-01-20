#ifndef WIZARDPEOPLE_RENDERING_VERTEX_HPP
#define WIZARDPEOPLE_RENDERING_VERTEX_HPP

#include <glm/vec3.hpp>

namespace rendering
{
	struct vertex
	{
		glm::vec3 position  = glm::vec3(0);
		glm::vec3 normal    = glm::vec3(0);
		glm::vec2 texCoord  = glm::vec3(0);
		glm::vec3 tangent   = glm::vec3(0);
		glm::vec3 bitangent = glm::vec3(0);
	};
}

#endif
