#ifndef GAME_BOARD_UPDATE_SYSTEM_HPP
#define GAME_BOARD_UPDATE_SYSTEM_HPP

#include "ecs/state.hpp"
#include <transforms/transform.hpp>
#include <core/game_input_manager.hpp>
#include <core/frame_timer.hpp>
#include <event/event_manager.hpp>
#include "components/game_piece.hpp"
#include "components/board.hpp"

class unit_death_event : public event
{
public:
	unit_death_event(entity_id _id) : mType(event::EVENT_TYPE::UNIT_DEATH), id(_id) {}
	entity_id id;
};

class board_update : public ecs::system_base, public event::Listener
{
public:

	board_update(core::game_input_manager& input, core::frame_timer& timer, event::EventManager& _event_manager) : m_input(input), m_timer(timer), event_manager(_event_manager)
	{
		// Add event subscriptions here
	}

	void HandleEvent(event::Event& event)
	{
		// Do nothing for now
	}

	void walk_unit(transforms::transform& transform, components::game_piece& game_piece, float delta)
	{
		transform.position += delta * game_piece.speed * game_piece.move_dir;
	}

	bool check_attacks(glm::vec2 location, std::vector<glm::vec2> targets)
	{	
		r.state.each_id<components::game_piece>([&](entity_id id, components::game_piece& game_piece) 
		{
			for (auto& target : targets)
			{
				if (game_piece.board_location == location + target)
				{
					return true;
				}
			}
		});

		return false;
	}

	void attack_targets(glm::vec2 location, std::vector<glm::vec2> targets, float damage)
	{
		r.state.each_id<components::game_piece>([&](entity_id id, components::game_piece& game_piece)
		{
			for (auto& target : targets)
			{
				if (game_piece.board_location == location + target)
				{
					game_piece.health -= damage;

					if (game_piece.health <= 0.f)
					{
						game_piece.state = components::UNIT_STATE::DYING;
					}
				}
			}
		});
	}

	glm::vec2 move_check(entity_id id, glm::vec2 start, glm::vec2 end, ecs::state& r_state)
	{
		int start_loc = static_cast<int>(start.y);
		int end_loc = static_cast<int>(end.y);
		glm::vec2 ret = start;

		if (end_loc > start_loc)
		{
			for (int current_loc = start_loc; current_loc <= end_loc; current_loc++)
			{
				r_state.each_id<components::game_piece>([&](entity_id id_two, components::game_piece& game_piece)
				{
					if (game_piece.board_location.x == ret.x && id != id_two)
					{
						if (current_loc == game_piece.board_location.y)
						{
							return ret;
						}
						else
						{
							ret.x = current_loc;
						}
					}
				});
			}
		}
		else
		{
			for (int current_loc = start_loc; current_loc >= end_loc; current_loc--)
			{
				r_state.each_id<components::game_piece>([&](entity_id id_two, components::game_piece& game_piece)
				{
					if (game_piece.board_location.x == ret.x && id != id_two)
					{
						if (current_loc == game_piece.board_location.y)
						{
							return ret;
						}
						else
						{
							ret.x = current_loc;
						}
					}
				});
			}
		}

		return ret;
	}

	void move_unit(components::game_piece& game_piece, glm::vec2 location)
	{
		game_piece.board_location = location;
	}

	virtual void update(ecs::state& r_state) override
	{
		float delta = m_timer.smoothed_delta_secs();

		// Do board combat every 1 second
		timer -= delta;
		if (timer <= 0.f)
		{
			timer = 1.f;

			// Update states to either attack or move depending on current board state
			r_state.each<components::game_piece, transforms::transform>([&](components::game_piece& game_piece, transforms::transform& transform)
			{
				
				if (check_attacks(game_piece.board_location, game_piece.attacks))
				{
					game_piece.state = components::UNIT_STATE::ATTACK;
				}
				else
				{
					game_piece.state = components::UNIT_STATE::MOVE;
				}
			});

			// If a unit can attack, attack now
			r_state.each<components::game_piece, transforms::transform>([&](components::game_piece& game_piece, transforms::transform& transform)
			{
				if (game_piece.state == components::UNIT_STATE::ATTACK || game_piece.state == components::UNIT_STATE::DYING)
				{
					// Attack animation here
					attack_targets(game_piece.board_location, game_piece.attacks, float damage);
				}
			});

			// If a unit is still alive after attacks and has a move state, check for legality and then move
			r_state.each_id<components::game_piece, transforms::transform>([&](entity_id, components::game_piece& game_piece, transforms::transform& transform)
			{
				if (game_piece.state == components::UNIT_STATE::MOVE)
				{
					glm::vec2 movement = move_check(entity_id id, game_piece.board_location, game_piece.board_location + game_piece.move_board, r_state);
					if (movement == game_piece.board_location)
					{
						game_piece.state = components::UNIT_STATE::WAIT;
					}
					else
					{
						move_unit(game_piece, movement);
					}
				}
			});
		}

		r_state.each_id< transforms::transform, components::game_piece>([&](entity_id id, transforms::transform& transform, components::game_piece& game_piece)
		{
			if (game_piece.state == components::UNIT_STATE::MOVE)
			{
				// Walking animation here
				walk_unit(transform, game_piece, delta);
			}
			if (game_piece.state == components::UNIT_STATE::DYING)
			{
				// Death animation here
				unit_death_event new_event(id);
				event_manager.BroadcastEvent(new_event);
			}
		});
	}

private:
	float timer = 1.f;
	core::game_input_manager& m_input;
	core::frame_timer& m_timer;
	event::EventManager& event_manager;
};

#endif