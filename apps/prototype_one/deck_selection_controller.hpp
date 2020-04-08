#ifndef __DECK_SELECTION_SYSTEM_HPP_
#define __DECK_SELECTION_SYSTEM_HPP_

#include <ecs/system_base.hpp>
#include <asset/scene_hydrater.hpp>
#include <core/game_input_manager.hpp>
#include <core/frame_limiter.hpp>
#include <asset/scene_hydrater.hpp>
#include "components/deck_selection.hpp"
#include "components/deck_option.hpp"
#include "components/board.hpp"
#include "game_util/player_specifics.hpp"

#include "components/card_enum.hpp"
#include "components/player.hpp"

class deck_selection_controller : public ecs::system_base
{
	glm::vec3 MovePosition { -9999.f, 0.f, 0.f };


public:
	deck_selection_controller(
		asset::scene_hydrater& hydrater,
		core::game_input_manager& input,
		core::frame_timer& timer);

	void initialize(ecs::state& state);
	void update(ecs::state& state);

private:
	asset::scene_hydrater& _hydrater;
	core::game_input_manager& _input;
	core::frame_timer& _timer;
	
	ecs::entity* _board{ nullptr };
	ecs::entity* _players[2]{ nullptr, nullptr };
	ecs::entity* _deck_selection{ nullptr };

	std::vector<std::set<components::card_enum>> _decks;
	std::vector<std::vector<glm::ivec2>> _preview_positions;

	void do_update(ecs::state& state, components::board& board);
	void hide_elements(ecs::state& state, components::board& board);

	void position_deck_option(
		components::deck_index deck_index,
		ecs::entity& option_entity,
		ecs::entity& player,
		components::deck_selection& deck_selection);

	void handle_player_selection(
		ecs::state& state,
		components::player& player,
		player_specific_data& player_specifics,
		components::deck_selection& deck_selection);

	void check_players_ready(ecs::state& state);

	void on_start(ecs::state& state);

	void build_deck_set(components::deck_index index, const std::vector<components::card_enum>& deck);

	void spawn_preview_units(ecs::state& state);

	static void handle_unit_transform(
		transforms::transform& board_t,
		transforms::transform& unit_t,
		components::game_piece& game_piece,
		components::board& board,
		entity_id board_id);

	bool can_run() const;
};



#endif
