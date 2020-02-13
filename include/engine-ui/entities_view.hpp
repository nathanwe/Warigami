#ifndef __ENTITIES_VIEW_HPP_
#define __ENTITIES_VIEW_HPP_

#include <cstdint>
#include <functional>
#include <string>
#include <engine-ui/view.hpp>
#include <event/event_manager.hpp>
#include "ecs/state.hpp"
#include "transforms/transform.hpp"
#include <ecs/ecs_types.hpp>

namespace engineui
{

	class entities_view : public view
	{
		static const std::uint32_t Width = 200;
		static const std::uint32_t Height = 200;

	public:
		entities_view(core::viewport& viewport, EventManager& events);
		void draw();

		void update(ecs::state state);

	private:
		std::list<entity_id> _entities;

		EventManager& _events;
	};

}

#endif
