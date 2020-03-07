#ifndef __RENDERABLE_TEXT_HPP_
#define __RENDERABLE_TEXT_HPP_

#include <cstdint>
#include <glm/vec3.hpp>
#include <ecs/component.hpp>
#include <glm/ext.hpp>

namespace rendering
{
	struct renderable_text : public ecs::component<renderable_text>
	{
		size_t string_hash{ 0 };
		glm::vec3 color{ 0.f };
		glm::ivec2 position {0};
        float scale {1.f};
		std::uint16_t size{ 12 };
	};
}

#endif