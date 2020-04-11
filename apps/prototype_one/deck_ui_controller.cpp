//
// Created by sava on 2020-03-07.
//

#include "deck_ui_controller.hpp"
#include "components/deck_ui.hpp"
#include "components/player.hpp"
#include "components/card.hpp"
#include "components/board.hpp"

deck_ui_controller::deck_ui_controller(asset::scene_hydrater &hydrater) : _hydrater(hydrater)
{}


void deck_ui_controller::initialize(ecs::state& state)
{
    _board = state.first<components::board>();
}

void deck_ui_controller::update(ecs::state &state)
{
    if (_board == nullptr) return;

    auto& board_component = _board->get_component<components::board>();

    state.each_id<transforms::transform, components::deck_ui>([&](entity_id deck_id, auto& deck_t, auto& deck) {
        if (board_component.state == components::game_state::gameplay)
            handle_deck_ui(state, deck, deck_t, deck_id);
        else
            hide_deck_ui(state, deck, deck_t);

        for (size_t i = 0; i < deck.child_count; ++i)
        {
            auto& e = state.find_entity(deck.children[i]);
            auto& t = e.get_component<transforms::transform>();
            t.is_matrix_dirty = true;
        }
    });
}

void deck_ui_controller::handle_deck_ui(
        ecs::state &state,
        components::deck_ui &deck,
        transforms::transform& deck_t,
        entity_id deck_id)
{
    deck_t.position = glm::vec3(0);
    deck_t.is_matrix_dirty = true;
    handle_card_entities(state, deck, deck_t, deck_id);
}

void deck_ui_controller::hide_deck_ui(ecs::state& state, components::deck_ui& deck, transforms::transform& deck_t)
{
    deck_t.position = MovePosition;
    deck_t.is_matrix_dirty = true;

    for (size_t i = 0; i < deck.child_count; ++i)
    {
        auto& e = state.find_entity(deck.children[i]);
        auto& t = e.get_component<transforms::transform>();
        t.is_matrix_dirty = true;
    }
}

void deck_ui_controller::handle_card_entities(
    ecs::state &state,
    components::deck_ui &deck_ui,
    transforms::transform& deck_ui_t,
    entity_id deck_id)
{
    state.each<components::player>([&](components::player &p) {
        for (size_t i = 0; i < components::player::MaxCards; ++i)
        {
            auto card_val = p.hand[i];
            auto& card_entity_id = p.team == 1 ? deck_ui.p1_hand[i] : deck_ui.p2_hand[i];

            if (card_entity_id && card_val != components::card_enum::NO_CARD)
            {
                // make sure the right card is spawned
                auto& card_entity = state.find_entity(card_entity_id.value());
                auto& card_component = card_entity.get_component<components::card>();

                if (card_component.card_type != card_val)
                {
                    _hydrater.remove_entity(card_entity);
                    auto& new_card = spawn_card(card_val);
                    auto& new_card_t = new_card.get_component<transforms::transform>();
                    deck_ui.replace_child(*card_entity_id, new_card.id());
                    card_entity_id = new_card.id();
                    position_card(i, new_card_t, p, deck_ui, deck_id);
                } else
                {
                    auto& card = state.find_entity(card_entity_id.value());
                    auto& card_t = card.get_component<transforms::transform>();
                    position_card(i, card_t, p, deck_ui, deck_id);
                }
            }
            else if (!card_entity_id && card_val != components::card_enum::NO_CARD)
            {
                auto& new_card = spawn_card(card_val);
                auto& new_card_t = new_card.get_component<transforms::transform>();
                card_entity_id = new_card.id();
                deck_ui.children[deck_ui.child_count++] = new_card.id();
                position_card(i, new_card_t, p, deck_ui, deck_id);
            }
            else if (card_entity_id && card_val == components::card_enum::NO_CARD)
            {
                deck_ui.remove_child(*card_entity_id);
                _hydrater.remove_entity(card_entity_id.value());
                card_entity_id = {};
            }
        }
    });
}

ecs::entity &deck_ui_controller::spawn_card(components::card_enum type)
{
    switch (type)
    {
        case components::card_enum::SCISSORLING_TWIN:
            return _hydrater.add_from_prototype("assets/prototypes/card_tandem.json");
        case components::card_enum::SCISSOR_TROOPER:
            return _hydrater.add_from_prototype("assets/prototypes/card_trooper.json");
        case components::card_enum::SCISSORLING_EGG:
            return _hydrater.add_from_prototype("assets/prototypes/card_egg.json");
        case components::card_enum::SCISSORLING:
            return _hydrater.add_from_prototype("assets/prototypes/card_scissorling.json");
        case components::card_enum::SCISSOR_WEBBER:
            return _hydrater.add_from_prototype("assets/prototypes/card_webber.json");
        case components::card_enum::SCISSOR_GOLIATH:
            return _hydrater.add_from_prototype("assets/prototypes/card_goliath.json");
        case components::card_enum ::SCISSOR_TITAN:
            return _hydrater.add_from_prototype("assets/prototypes/card_dragon.json");
        case components::card_enum::SCISSOR_QUEEN:
            return _hydrater.add_from_prototype("assets/prototypes/card_queen.json"); 

        case components::card_enum::LIGHT_TANK_SOLDIER:
            return _hydrater.add_from_prototype("assets/prototypes/card_light_tank_soldier.json");
        case components::card_enum::LIGHT_RANGE_SOLDIER:
            return _hydrater.add_from_prototype("assets/prototypes/card_light_range_soldier.json");
        case components::card_enum::LIGHT_FAST_SOLDIER:
            return _hydrater.add_from_prototype("assets/prototypes/card_light_fast_soldier.json");
        case components::card_enum::MEDIUM_TANK_SOLDIER:
            return _hydrater.add_from_prototype("assets/prototypes/card_medium_tank_soldier.json");
        case components::card_enum::MEDIUM_RANGE_SOLDIER:
            return _hydrater.add_from_prototype("assets/prototypes/card_medium_range_soldier.json");
        case components::card_enum::MEDIUM_FAST_SOLDIER:
            return _hydrater.add_from_prototype("assets/prototypes/card_medium_fast_soldier.json");
        case components::card_enum::HEAVY_TANK_SOLDIER:
            return _hydrater.add_from_prototype("assets/prototypes/card_heavy_tank_soldier.json");
        case components::card_enum::HEAVY_RANGE_SOLDIER:
            return _hydrater.add_from_prototype("assets/prototypes/card_heavy_range_soldier.json");
        case components::card_enum::HEAVY_FAST_SOLDIER:
            return _hydrater.add_from_prototype("assets/prototypes/card_heavy_fast_soldier.json");

        case components::card_enum::ERASER:
            return _hydrater.add_from_prototype("assets/prototypes/card_eraser.json");
        case components::card_enum::TROLL:
            return _hydrater.add_from_prototype("assets/prototypes/card_troll.json");
        case components::card_enum::WIZARD:
            return _hydrater.add_from_prototype("assets/prototypes/card_wizard.json");
        case components::card_enum::ARCHER:
            return _hydrater.add_from_prototype("assets/prototypes/card_archer.json");
        case components::card_enum::UNICORN:
            return _hydrater.add_from_prototype("assets/prototypes/card_unicorn.json");
        case components::card_enum::KNIGHT:
            return _hydrater.add_from_prototype("assets/prototypes/card_knight.json");

        default:
            return _hydrater.add_from_prototype("assets/prototypes/card_dragon.json");
    }
}

void
deck_ui_controller::position_card(
        size_t card_index,
        transforms::transform &card_t,
        components::player &player,
        components::deck_ui &deck,
        entity_id deck_id)
{
    auto start_x = player.team == 1
                    ? deck.min_card_x
                    : deck.deck_gap;

    auto x_increment = (deck.max_card_x - deck.deck_gap) /
            (components::player::MaxCards - 1.f);

    card_t.has_parent = true;
    card_t.parent = deck_id;
    card_t.position.x = start_x + x_increment * card_index;
    card_t.position.y = deck.card_y;
    card_t.position.z = -deck.card_depth;
    card_t.is_matrix_dirty = true;
}

void
deck_ui_controller::position_tooltip(
        size_t card_index,
        transforms::transform &card_t,
        components::player &player,
        components::deck_ui &deck,
        entity_id deck_id)
{
    float start_x = player.team == 1
                    ? deck.min_card_x
                    : deck.deck_gap;

    float x_increment = (deck.max_card_x - deck.deck_gap) /
                        (components::player::MaxCards - 1.f);

    card_t.has_parent = true;
    card_t.parent = deck_id;
    card_t.position.x = start_x + x_increment * card_index + 0.25f;
    card_t.position.y = deck.card_y + 2.5;
    card_t.position.z = -deck.card_depth;
    card_t.is_matrix_dirty = true;
}

