#ifndef __AI_SYSTEM_HPP_
#define __AI_SYSTEM_HPP_

#include <ecs/system_base.hpp>
#include <ecs/state.hpp>
#include <algorithm>
#include <random>

#include "components/board.hpp"
#include "components/pause.hpp"
#include "components/player.hpp" 


class AI_system : public ecs::system_base
{
public:
	AI_system(core::frame_timer& timer, asset::scene_hydrater& hydrater)
		: _timer(timer)
		, _hydrater(hydrater)
	{
	}

	struct row_data {
		int enemy_unit_count = 0;
		int freindly_unit_count = 0;
		int avalible_energy = 0;
		components::card_enum a_enemy;
		int a_enemy_location;
		components::card_enum a_freind;
		int a_freind_location;
		int my_teritory;
		bool enemy_wizelf;
		bool friendly_wizelf;
	};
	
	void evaluate_state_for_info(ecs::state& state, components::player& player_in) {
		for (int i = 0; i < 5; i++) {
			info[i].enemy_unit_count = 0;
			info[i].freindly_unit_count = 0;
			info[i].avalible_energy = 0;
			info[i].my_teritory = 0;
			info[i].friendly_wizelf = false;
			info[i].enemy_wizelf = false;
		}
		//count terain
		state.each_id<components::board_square, components::terrain>([&]
		(entity_id board_square_id, components::board_square& square, components::terrain& terrain)
			{
				if (square.team == player_in.team) {
					info[square.x].my_teritory++;
					if (terrain.growth_stage == 3) {
						info[square.x].avalible_energy++;
					}
				}
			});
		//count units
		state.each<components::game_piece>([&]
		(components::game_piece& piece)
			{
				if (piece.team == player_in.team) {
					info[piece.board_destination.x].freindly_unit_count++;
					info[piece.board_destination.x].a_freind = piece.piece_type;
					info[piece.board_destination.x].a_freind_location = piece.board_destination.y * player_in.team + (1- player_in.team)*4;
					if (piece.piece_type == components::card_enum::ARCHER || piece.piece_type == components::card_enum::WIZARD) {
						info[piece.board_destination.x].friendly_wizelf = true;
					}
				}
				else {
					info[piece.board_destination.x].enemy_unit_count++;
					info[piece.board_destination.x].a_enemy = piece.piece_type;
					info[piece.board_destination.x].a_enemy_location = piece.board_destination.y * player_in.team + (1 - player_in.team) * 4;
					if (piece.piece_type == components::card_enum::ARCHER || piece.piece_type == components::card_enum::WIZARD) {
						info[piece.board_destination.x].enemy_wizelf = true;
					}
				}
			});
		//count orbs
		state.each<components::energy_ball>([&]
		(components::energy_ball& ball)
			{
				if (ball.team == player_in.team) {
					info[ball.lane].avalible_energy++;
				}
			});


	}
	float sigmoidish_energy_multiplyer(int current_energy) {
		if (current_energy < 4) {
			return 1.0f;
		}
		else if (current_energy > 7) {
			return 0.0f;

		}
		else {
			return 2.0f - current_energy / 4.0f;
		}

	}
	float lane_energy_utiltiy(int current_energy, int lane_energy) {
		return lane_energy * sigmoidish_energy_multiplyer(current_energy);
	}
	

	float unit_utility(int lane, int hand_pos, components::player& player_in) 
	{
		if (hand_pos == 4) 
		{
			return 0.0f;
		}
		if (info[lane].enemy_wizelf && info[lane].a_freind_location >= 6 && info[lane].friendly_wizelf){
			if(player_in.hand[hand_pos] == components::card_enum::WIZARD|| player_in.hand[hand_pos] == components::card_enum::ARCHER)
				{
					return 3.0f;
				}
		}
		if (info[lane].enemy_unit_count == 1 && info[lane].a_enemy_location <= 1)
		{
			return -1.0f;
		}
		if (info[lane].freindly_unit_count == 1) 
		{
			return -1.0f;
		}
		if (info[lane].enemy_unit_count >= 2) 
		{
			return -1.0f;
		}
		if (info[lane].enemy_unit_count == 0 && info[lane].freindly_unit_count == 0) 
		{
			return 1.0f;
		}
		
		if (info[lane].enemy_unit_count == 1 && info[lane].freindly_unit_count == 0) 
		{
			
			int enemy_num = (int)info[lane].a_enemy - 20;
			int friend_num = (int)player_in.hand[hand_pos] - 20;
			if (enemy_num - friend_num == 2) {
				int breakpoint = 69;
			}
			int machup_num = (enemy_num - friend_num + 6) % 6;
			if (friend_num % 2 == 0) 
			{
				if (machup_num == 0) {
					return 0.5f;
				}
				else if (machup_num <= 3) {
					return 0.0f;
				}
				else if (machup_num > 3) {
					return 1.0f;
				}
			}
			else if (friend_num % 2 == 1) {
				if (machup_num == 0) {
					return 0.5f;
				}
				else if (machup_num <= 2) {
					return 0.0f;
				}
				else if (machup_num > 2)
				{
					return 1.0f;
				}
			}
			/*
			switch (player_in.hand[hand_pos]) 
			{
			case components::card_enum::ERASER:
				switch (info[lane].a_enemy)
				{
				case components::card_enum::ERASER:
					return 0.5f;
				}
			}
			*/

		}
		return -69.0f;
		
		

	}
	float cost_penalty(int hand_pos, components::player& player_in) {
		if (hand_pos == 4) {
			return 0.0f;
		}
		if (player_in.energy < components::card_costanamos[(int)player_in.hand[hand_pos]]) {
			return 999;
		}
		return 0.1f*components::card_costanamos[(int)player_in.hand[hand_pos]];
	}
	
	
	void update_weights(components::player& player_in) {
		for (int lane = 0; lane < 5; lane++) {
			float lane_utiltiy = lane_energy_utiltiy(player_in.energy, info[lane].avalible_energy);
			float territory_gain_utility = 9.0f - info[lane].my_teritory;
			float distance_penalty = 0.1f * std::abs(lane - player_in.selected_row);

			for (int hand_pos = 0; hand_pos < 5; hand_pos++) {
				float cost_penalty_val = cost_penalty(hand_pos, player_in);
				float unit_utility_val = unit_utility(lane, hand_pos, player_in);
				behavior_weights[lane][hand_pos] = lane_utiltiy + unit_utility_val * territory_gain_utility - cost_penalty_val - distance_penalty;
			}
		}
	}
	void choose_best(float dificulty, float sdev) {
		float best1 = -99999;
		float best2 = -99999;
		float best3 = -99999;
		float best25 = 99999;
		for (int lane = 0; lane < 5; lane++) {
			for (int hand_pos = 0; hand_pos < 5; hand_pos++) {
				if (behavior_weights[lane][hand_pos] >= best1) {
					best3 = best2;
					best2 = best1;
					best1 = behavior_weights[lane][hand_pos];
				}
				else if (behavior_weights[lane][hand_pos] >= best2) {
					best3 = best2;
					best2 = behavior_weights[lane][hand_pos];
				}
				else if (behavior_weights[lane][hand_pos] >= best3) {
					best3 = behavior_weights[lane][hand_pos];
				}
				if (behavior_weights[lane][hand_pos] <= best25) {
					best25 = behavior_weights[lane][hand_pos];
				}
			}
		}
		std::vector<std::pair<int, int>> best_options;

		static std::random_device rd;
		auto rng = std::default_random_engine{ rd() };
		std::normal_distribution<float> dice(dificulty,sdev);
		auto gauss = std::bind(dice, rng);
		float da_gauss = gauss();
		float chosen = best1;
		
		if (da_gauss < 1.5) {
			chosen = best1;
		}
		else if (da_gauss < 2.5) {
			chosen = best2;
		}
		else if (da_gauss < 3.5) {
			chosen = best3;
		}
		else{
			chosen = best25;
		}

		for (int lane = 0; lane < 5; lane++) {
			for (int hand_pos = 0; hand_pos < 5; hand_pos++) {
				if (behavior_weights[lane][hand_pos] == chosen) {
					best_options.push_back(std::pair<int, int>(lane, hand_pos));
				}
			}
		}

		std::shuffle(std::begin(best_options), std::end(best_options), rng);

		chosen_lane = best_options.begin()->first;
		chosen_unit = best_options.begin()->second;
	}
	void update(ecs::state& state)
	{
		// Skip system when paused
		auto e = state.first<components::pause>([](auto& pause)
			{
				return true;
			});
		if (e && e->get_component<components::pause>().is_game_paused)
		{
			return;
		}
		std::random_device rd;
		auto rng = std::default_random_engine{ rd() };
		std::uniform_int_distribution<int> dice(0, 8000);
		auto d8000 = std::bind(dice, rng);
		std::uniform_int_distribution<int> coin(0, 2);
		auto d2 = std::bind(coin, rng);
		state.each_id<transforms::transform, components::board>(
			[&](entity_id board_id, transforms::transform& board_t, components::board& board) 
			{				
				state.each<components::player>([&](components::player& player)
				{
					if (player.controlled_by_AI == 2)
					{

						player.pull = true;
						if (board.did_tick_elapse || board.did_half_tick_elapse)
						{
							evaluate_state_for_info(state, player);
							for (int lane = 0; lane < 5; lane++) {
								float lane_utiltiy = lane_energy_utiltiy(player.energy, info[lane].avalible_energy);
								float territory_gain_utility = 9.0f - info[lane].my_teritory;
								float distance_penalty = 0.01f * std::abs(lane - player.selected_row);

								for (int hand_pos = 0; hand_pos < 5; hand_pos++) {
									float cost_penalty_val = cost_penalty(hand_pos, player);
									float unit_utility_val = unit_utility(lane, hand_pos, player);
									behavior_weights[lane][hand_pos] = lane_utiltiy + unit_utility_val * territory_gain_utility - cost_penalty_val - distance_penalty;
								}
							}						
							//update_weights(player);
							choose_best(player.AI_dificulty, player.AI_dificulty_sdev);
							player.pull = false;

							if (chosen_lane > player.selected_row) {
								player.selected_row++;
							}
							if (chosen_lane < player.selected_row) {
								player.selected_row--;
							}
							if (chosen_lane == player.selected_row) {
								if (chosen_unit != 4) {
									player.place_card(chosen_unit, _timer.total_s(), state, board.spawner, _hydrater);
								}
							}
						}
					}
					if (player.controlled_by_AI == 1)
					{
						player.pull = true;
						if (board.did_tick_elapse || board.did_half_tick_elapse)
						{
							if (player.selected_row >= board.columns - 1) {
								player.AI_movement_direction = -1;
							}
							if (player.selected_row <= 0) {
								player.AI_movement_direction = 1;
							}
							player.selected_row += player.AI_movement_direction;
							if (player.selected_row != board.columns - 1 && d2() == 1) {
								player.place_card(0, _timer.total_s(), state, board.spawner, _hydrater);
							}



							// extra energy

							int p1_sq = 0;
							int p2_sq = 0;
							state.each_id<components::board_square, components::terrain>([&]
							(entity_id board_square_id, components::board_square& square, components::terrain& terrain)
								{
									if (square.team == 1) {
										p1_sq++;
									}
									else if (square.team == -1) {
										p2_sq++;
									}
								});

							state.each_id<components::board_square, components::terrain>([&]
							(entity_id board_square_id, components::board_square& square, components::terrain& terrain) {
									if (d8000() < (player.team == 1 ? p2_sq * p2_sq : p1_sq * p1_sq) && terrain.type == components::TERRAIN_ENUM::NONE && square.team == player.team) {
										terrain.type = components::TERRAIN_ENUM::ENERGY_FLOWER;
										terrain.team = 0.0f;
										terrain.damage = -1;
										terrain.duration = -1;
										terrain.growth_stage = 7;
									}

								});
						}
					}
					
				});					
			});


	}

private:
	core::frame_timer& _timer;
	asset::scene_hydrater& _hydrater;
	row_data info[5];
	float behavior_weights[5][5];
	int chosen_lane;
	int chosen_unit;
};

#endif