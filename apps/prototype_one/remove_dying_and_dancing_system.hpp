#ifndef REMOVE_DYING_AND_DANCING_SYSTEM_HPP
#define REMOVE_DYING_AND_DANCING_SYSTEM_HPP


#include "ecs/state.hpp"
#include <transforms/transform.hpp>
#include <core/game_input_manager.hpp>
#include <core/frame_timer.hpp>
#include <event/event_manager.hpp>
#include "components/game_piece.hpp"
#include "components/board.hpp"
#include "components/board_square.hpp"
#include "combat.hpp"
#include "components/card_enum.hpp"
#include "components/terrain.hpp"
#include "components/to_spawn.hpp"
#include <algorithm>
#include <math.h>


class unit_death_event : public event::Event
{
public:
    explicit unit_death_event(entity_id _id) : event::Event(event::EVENT_TYPE::UNIT_DEATH), id(_id)
    {}

    entity_id id;
    std::vector<entity_id> children;
};

class remove_dying_and_dancing : public ecs::system_base, public event::Listener
{
public:

    remove_dying_and_dancing(
            core::game_input_manager &input,
            core::frame_timer &timer,
            event::EventManager &_event_manager,
            asset::scene_hydrater &_hydrater,
            combats::combat_resolution &_resolver)
            : m_input(input), m_timer(timer), event_manager(_event_manager), hydrater(_hydrater), resolver(_resolver)
    {
        event_manager.Subscribe(this, event::EVENT_TYPE::UNIT_DEATH);
    }

    void HandleEvent(event::Event &event) override
    {
        if (event.mType == event::EVENT_TYPE::UNIT_DEATH)
        {
            // Delete any children of the unit
            for (entity_id id : ((unit_death_event&)event).children) {
                hydrater.remove_entity(id);
            }
            // Delete unit
            hydrater.remove_entity(((unit_death_event &) event).id);
        }
    }

    void update(ecs::state &r_state) override
    {
        r_state.each_id<transforms::transform, components::board>(
                [&](entity_id board_id, transforms::transform& board_t, components::board&board) {                    
                if (!(board.state == components::game_state::gameplay || board.state == components::game_state::countdown)) return;

                    if (board.did_tick_elapse)
                        kill_dying_and_dancing(r_state);
                });
    }

private:
    core::game_input_manager &m_input;
    core::frame_timer &m_timer;
    event::EventManager &event_manager;
    asset::scene_hydrater &hydrater;
    combats::combat_resolution &resolver;   

    void kill_dying_and_dancing(ecs::state& r_state) {
        r_state.each_id<transforms::transform, components::game_piece>(
            [&](entity_id unit_id, auto& unit_t, auto& unit) {
            if (unit.state == components::UNIT_STATE::DYING || unit.state == components::UNIT_STATE::DANCING)
            {
                //unit.state = components::UNIT_STATE::DEAD;
                unit_death_event new_event(unit_id);
                // Delete also the health spheres
                for (ecs::entity e : unit.health_points) {
                    new_event.children.push_back(e.id());
                }
                event_manager.BroadcastEvent(new_event);
            }

            });
    }

   
};

#endif
