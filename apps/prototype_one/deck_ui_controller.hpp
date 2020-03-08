//
// Created by sava on 2020-03-07.
//

#ifndef WIZARDENGINE_DECK_UI_CONTROLLER_HPP
#define WIZARDENGINE_DECK_UI_CONTROLLER_HPP

#include <ecs/system_base.hpp>
#include <asset/scene_hydrater.hpp>
#include <transforms/transform.hpp>
#include "components/deck_ui.hpp"

class deck_ui_controller : public ecs::system_base
{
public:
    deck_ui_controller(asset::scene_hydrater &hydrater);

    void update(ecs::state& state) override;

private:
    asset::scene_hydrater& _hydrater;

    void handle_deck_ui(ecs::state &state,
                        components::deck_ui &deck,
                        transforms::transform& deck_t,
                        entity_id deck_id);


    void handle_card_entities(ecs::state &state,
                              components::deck_ui &deck_ui,
                              transforms::transform& deck_ui_t,
                              entity_id deck_id);

    ecs::entity& spawn_card(components::card_enum type);

    static void position_card(
            size_t card_index,
            transforms::transform& card_t,
            components::player& player,
            components::deck_ui& deck,
            entity_id deck_id);
};


#endif //WIZARDENGINE_DECK_UI_CONTROLLER_HPP
