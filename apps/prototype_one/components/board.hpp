#ifndef GAME_COMPONENTS_BOARD_HPP
#define GAME_COMPONENTS_BOARD_HPP

#include <vector>

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "component_bits.hpp"
#include "board_square.hpp"
#include "transforms/transform.hpp"
#include "to_spawn.hpp"

#include <glm/glm.hpp>

namespace components
{
    enum class game_state
    {
        gameplay,
        deck_selection,
        game_over,
        paused
    };

	struct board : ecs::component<board>
	{
		std::vector< std::vector< int > > board_state; // 
		int rows;
		int columns;
		float cell_width{ 1.f };
		float cell_height{ 1.f };

        float tick_time{ 1.f };
        float tick_time_remaining{ tick_time };
        float timer_t{ 0.f };
        float ticker_dt{ 0.f };
        bool did_tick_elapse{ false };

        game_state state{ game_state::gameplay };

        std::vector<to_spawn> spawner;

        // Helper function for mapping transforms to board space
        glm::vec3 grid_to_board(glm::vec2& coordinates, transforms::transform& board_t)
        {
            float total_w = rows;
            float total_h = columns;

            auto world_x = coordinates.x - (total_h / 2.f) + 0.5f;
            auto world_z = coordinates.y - (total_w / 2.f) + 0.5f;

            return glm::vec3(world_x, 1.8f, world_z);
        }

        void print()
        {
            for (int i = 0; i < board_state.size(); i++)
            {
                for (int j = 0; j < board_state[i].size(); j++)
                {
                    if (board_state[i][j] > 0)
                        std::cout << board_state[i][j] << "\t";
                    else
                        std::cout << board_state[i][j] << "\t";
                }

                std::cout << std::endl;
            }

            std::cout << std::endl;
        }

        void clear_state()
        {
            for (auto& row : board_state)
                for (auto& cell : row)
                    cell = 0;
        }
	};
}

#endif
