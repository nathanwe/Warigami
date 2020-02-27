#ifndef GAME_BOARD_UPDATE_SYSTEM_HPP
#define GAME_BOARD_UPDATE_SYSTEM_HPP

#include "ecs/state.hpp"
#include <transforms/transform.hpp>
#include <core/game_input_manager.hpp>
#include <core/frame_timer.hpp>
#include <event/event_manager.hpp>
#include "components/game_piece.hpp"
#include "components/board.hpp"


class board_update : public ecs::system_base, public event::Listener
{
public:

	board_update(core::game_input_manager& input, core::frame_timer& timer, event::EventManager& _event_manager) : m_input(input), m_timer(timer), event_manager(_event_manager)
	{
		event_manager.Subscribe(this, event::EVENT_TYPE::TOGGLE_NOCLIP);
	}

	void HandleEvent(event::Event& event)
	{
		if (event.mType == event::EVENT_TYPE::TOGGLE_NOCLIP)
		{
			std::cerr << "Event recieved" << std::endl;
		}
	}

	void walk(transforms::transform& transform, components::game_piece& game_piece, float delta)
	{
		transform.position += delta * game_piece.speed * game_piece.move_dir;
	}

	virtual void update(ecs::state& r_state) override
	{
		float delta = m_timer.smoothed_delta_secs();

		// Do board combat every 1 second
		timer -= delta;
		if (timer <= 0.f)
		{
			timer = 1.f;

			// Update the positions of all pieces on the board
			r_state.each<components::board>([&](components::board& board)
			{
				std::vector< std::vector< std::pair< entity_id, entity_id > > > new_board;
				for (int i = 0; i < board.rows; i++)
				{
					for (int j = 0; j < board.columns; j++)
					{
						if (board.board_state[i][j].second >= 0)
						{
							ecs::entity ent = r_state.find_entity(board.board_state[i][j].second);
							components::game_piece& piece = ent.get_component<game_piece>();
							if (!piece.moved)
							{
								piece.moved = true;
								new_board[i + piece.move_board.x][j + piece.move_board.y].second = ent.id();
							}
						}
					}
				}
			});
			r_state.each<components::game_piece>([&](components::game_piece& game_piece)
			{

			});
		}
		else
		{
			r_state.each< transforms::transform, components::game_piece>([&](transforms::transform& transform, components::game_piece& game_piece)
			{
				walk(transform, game_piece, delta);
			});
		}
	}

private:
	float timer = 1.f;
	core::game_input_manager& m_input;
	core::frame_timer& m_timer;
	event::EventManager& event_manager;
};

#endif