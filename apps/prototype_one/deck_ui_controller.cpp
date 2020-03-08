//
// Created by sava on 2020-03-07.
//

#include "deck_ui_controller.hpp"
#include "components/deck_ui.hpp"
#include "components/player.hpp"
#include "components/card.hpp"


deck_ui_controller::deck_ui_controller(asset::scene_hydrater &hydrater) : _hydrater(hydrater)
{}


void deck_ui_controller::update(ecs::state &state)
{
    state.each_id<transforms::transform, components::deck_ui>([&](entity_id deck_id, auto& deck_t, auto& deck) {
        handle_deck_ui(state, deck, deck_t, deck_id);
    });
}

void deck_ui_controller::handle_deck_ui(
        ecs::state &state,
        components::deck_ui &deck,
        transforms::transform& deck_t,
        entity_id deck_id)
{
    handle_card_entities(state, deck, deck_t, deck_id);
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
                position_card(i, new_card_t, p, deck_ui, deck_id);
            }
            else if (card_entity_id && card_val == components::card_enum::NO_CARD)
            {
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
        case components::card_enum::DRAGON:
            return _hydrater.add_from_prototype("assets/prototypes/card_dragon.json");
        case components::card_enum::ERATHOR:
            return _hydrater.add_from_prototype("assets/prototypes/card_erathor.json");
        case components::card_enum::MATCH:
            return _hydrater.add_from_prototype("assets/prototypes/card_matchgirl.json");
        case components::card_enum::SCISSORLING:
            return _hydrater.add_from_prototype("assets/prototypes/card_scissorling.json");
        case components::card_enum::WIZARD:
            return _hydrater.add_from_prototype("assets/prototypes/card_wizard.json");


        // TEMP:
        case components::card_enum::BASIC_FAST:
            return _hydrater.add_from_prototype("assets/prototypes/card_scissorling.json");
        case components::card_enum ::BASIC_MELEE:
            return _hydrater.add_from_prototype("assets/prototypes/card_erathor.json");
        case components::card_enum::BASIC_RANGE:
            return _hydrater.add_from_prototype("assets/prototypes/card_dragon.json");
        //////

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
    float start_x = player.team == 1
                    ? deck.min_card_x
                    : deck.deck_gap;

    float x_increment = (deck.max_card_x - deck.deck_gap) /
            (components::player::MaxCards - 1.f);

    card_t.has_parent = true;
    card_t.parent = deck_id;
    card_t.position.x = start_x + x_increment * card_index;
    card_t.position.y = deck.card_y;
    card_t.position.z = -deck.card_depth;
    card_t.is_matrix_dirty = true;
}

