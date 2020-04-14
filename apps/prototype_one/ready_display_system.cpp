#include "ready_display_system.hpp"
#include "components/ready_display.hpp"
#include "components/player.hpp"
#include <transforms/transform.hpp>
#include <rendering/renderable_text.hpp>
#include "components/board.hpp"



ready_display_system::ready_display_system(core::glfw_context& glfw)
	: _glfw(glfw)
	, _half_screen(glfw.width() / 2)
{
}

void ready_display_system::initialize(ecs::state& state)
{
	_board = state.first<components::board>();

	_players[0] = state.first<components::player>([&](components::player& p) {
		return p.team == 1.f;
	});

	_players[1] = state.first<components::player>([&](components::player& p) {
		return p.team == -1.f;
	});
}

void ready_display_system::update(ecs::state& state)
{
	state.each<components::ready_display>([&](components::ready_display& ready_dispaly) {
		auto& board = _board->get_component<components::board>();
		auto& p1 = _players[0]->get_component<components::player>();
		auto& p2 = _players[1]->get_component<components::player>();

		ready_dispaly.is_ready = ready_dispaly.team == 1.f ? p1.is_ready : p2.is_ready;
		auto& ready_entity = state.find_entity(ready_dispaly.ready_entity);
		auto& back_entity = state.find_entity(ready_dispaly.back_entity);
		auto& ready_t = ready_entity.get_component<transforms::transform>();
		auto& back_t = back_entity.get_component<transforms::transform>();
		auto& ready_text = ready_entity.get_component<rendering::renderable_text>();
		auto& back_text = back_entity.get_component<rendering::renderable_text>();

		ready_t.is_matrix_dirty = true;
		back_t.is_matrix_dirty = true;		
				
		//todo: magic numbers
		auto ratio = _glfw.width() / 1366.f;
		auto offset = ready_dispaly.team == 1.f ? 70  : _glfw.width() - 110.f;
		auto button_x = ready_dispaly.team == 1.f ? -16.f - (1.2f)  : 13.f + (1.35f);
		auto button_y = -9.5f;
		auto button_z = -24.5f;
		auto text_y = 16;		
		auto hide_x = 9999;

		if (board.state != components::game_state::deck_selection)
		{
			ready_text.position.x = hide_x;
			ready_t.position = HidePosition;
			back_text.position.x = hide_x;
			back_t.position = HidePosition;			
		}
		else if (ready_dispaly.is_ready)
		{
			back_text.position = {offset, text_y};
			ready_text.position.x = hide_x;
			back_t.position = glm::vec3(button_x, button_y, button_z);
			ready_t.position = HidePosition;			
		}
		else
		{
			ready_text.position = { offset, text_y };
			back_text.position.x = hide_x;
			ready_t.position = glm::vec3(button_x, button_y, button_z);
			back_t.position = HidePosition;
		}
				
		for (size_t i = 0; i < ready_dispaly.child_count; ++i)
		{
			auto& e = state.find_entity(ready_dispaly.children[i]);
			auto& t = e.get_component<transforms::transform>();
			t.is_matrix_dirty = true;
		}
	});
}
