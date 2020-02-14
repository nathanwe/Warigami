#ifndef __ENTITIES_VIEW_HPP_
#define __ENTITIES_VIEW_HPP_

#include <list>
#include <cstdint>
#include <functional>
#include <string>
#include <engine-ui/view.hpp>
#include <event/event_manager.hpp>
#include "ecs/state.hpp"
#include "transforms/transform.hpp"
#include <ecs/ecs_types.hpp>
#include "engine-ui/transform_tree.hpp"

namespace engineui
{

	class entities_view : public view
	{
		static const std::uint32_t Width = 200;
		static const std::uint32_t Height = 200;

		static const std::uint32_t InspectorWidth = 200;
		static const std::uint32_t InspectorHeight = 200;

	public:
		entities_view(core::viewport& viewport, EventManager& events, ecs::state& r_ecs_state);

		void draw();

		void update(ecs::state state);

	private:
		std::list<entity_id> _entities;

		EventManager& _events;

		ecs::state& m_r_ecs_state;
		transform_tree tree;
	};

}

#endif
