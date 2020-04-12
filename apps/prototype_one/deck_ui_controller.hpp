//
// Created by sava on 2020-03-07.
//

#ifndef WIZARDENGINE_DECK_UI_CONTROLLER_HPP
#define WIZARDENGINE_DECK_UI_CONTROLLER_HPP

#include <ecs/system_base.hpp>
#include <asset/scene_hydrater.hpp>
#include <transforms/transform.hpp>
#include "components/deck_ui.hpp"
#include "game_util/card_spawner.hpp"

class deck_ui_controller : public ecs::system_base
{
	glm::vec3 MovePosition{ -9999.f, 0.f, 0.f };


public:
	deck_ui_controller(card_spawner& card_spawner);

	void initialize(ecs::state& state) override;
	void update(ecs::state& state) override;

private:	
	card_spawner& _card_spawner;

	ecs::entity* _board{ nullptr };


	void handle_deck_ui(
		ecs::state& state,
		components::deck_ui& deck,
		transforms::transform& deck_t,
		entity_id deck_id);

	void hide_deck_ui(
		ecs::state& state,
		components::deck_ui& deck,
		transforms::transform& deck_t);

	void handle_card_entities(ecs::state& state,
		components::deck_ui& deck_ui,
		transforms::transform& deck_ui_t,
		entity_id deck_id);

	static void position_card(
		size_t card_index,
		transforms::transform& card_t,
		components::player& player,
		components::deck_ui& deck,
		entity_id deck_id);

	static void position_tooltip(
		size_t card_index,
		transforms::transform& card_t,
		components::player& player,
		components::deck_ui& deck,
		entity_id deck_id);
};


#endif //WIZARDENGINE_DECK_UI_CONTROLLER_HPP
