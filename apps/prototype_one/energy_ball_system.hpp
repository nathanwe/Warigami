#ifndef __ENREGY_BALL_SYSTEM_HPP_
#define __ENREGY_BALL_SYSTEM_HPP_

#include <glm/vec3.hpp>
#include <asset/scene_hydrater.hpp>
#include <core/game_input_manager.hpp>
#include <core/frame_timer.hpp>
#include <ecs/system_base.hpp>
#include <ecs/entity.hpp>
#include "components/player.hpp"
#include "components/selection_arrow.hpp"
#include "game_util/player_specifics.hpp"


class energy_ball_system : public ecs::system_base
{
public:
	energy_ball_system(
		core::game_input_manager& input,
		asset::scene_hydrater& hydrater,
		core::frame_timer& timer);

	void initialize(ecs::state& state) override;
	void update(ecs::state& state) override;

private:
	core::game_input_manager& _input;
	asset::scene_hydrater& _hydrater;
	core::frame_timer& _timer;

	ecs::entity* _players[2]{ nullptr, nullptr };
	ecs::entity* _selectors[2]{ nullptr, nullptr };


	glm::vec3 find_target_position(components::selection_arrow& arrow);
};


#endif