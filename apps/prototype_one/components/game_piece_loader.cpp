#include "game_piece_loader.hpp"
#include <iostream>

void components::game_piece_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto entity = asset_loader_node.entity_resource.entity;
	auto entity_data = asset_loader_node.entity_resource.entity_data;

	auto& json = entity_data->component_data(components::game_piece::component_bitshift);
	auto& gp = entity->get_component<game_piece>();

	gp.speed = json.value("speed", 1);
	gp.damage = json.value("damage", 1);
	gp.health = json.value("health", 1);
	gp.max_health = json.value("health", 1);
	gp.team = json.value("team", 1);
	gp.give_points = gp.health;

	if (json.find("board_location") != json.end())
	{
		gp.board_source = glm::ivec2(
			json["board_location"][0].get<float>(),
			json["board_location"][1].get<float>());

		gp.continuous_board_location = gp.board_source;
	}


	if (json.find("move_board") != json.end())
	{
		gp.move_board = glm::vec2(
			json["move_board"][0].get<float>(),
			json["move_board"][1].get<float>());
		gp.move_board = gp.move_board * gp.team;
	}

	if (json.find("move_world") != json.end())
	{
		gp.move_world = glm::vec3(
			json["move_world"][0].get<float>(),
			json["move_world"][1].get<float>(),
			json["move_world"][2].get<float>());
		gp.move_world = gp.move_world * (float)gp.team;
	}

	if (json.find("attacks") != json.end())
	{
		for (auto& vec : json["attacks"])
		{
			gp.attacks.push_back(glm::ivec2(vec[0].get<int>(), vec[1].get<int>()) * (int)gp.team);
		}
	}

	if (json.find("effects") != json.end())
	{
		for (auto& vec : json["effects"])
		{
			gp.effects.push_back(vec);
		}
	}

	auto it = json.find("piece_type");
	if (it != json.end())
	{
		std::string val = it->get<std::string>();
		if (val == "spider_goliath")
		{
			gp.piece_type = components::card_enum::SCISSOR_GOLIATH;
		}
		else if (val == "spider_queen")
		{
			gp.piece_type = components::card_enum::SCISSOR_QUEEN;
		}
		else if (val == "spider_titan")
		{
			gp.piece_type = components::card_enum::SCISSOR_TITAN;
		}
		else if (val == "spider_trooper")
		{
			gp.piece_type = components::card_enum::SCISSOR_TROOPER;
		}
		else if (val == "spider_webber")
		{
			gp.piece_type = components::card_enum::SCISSOR_WEBBER;
		}
		else if (val == "spider_ling")
		{
			gp.piece_type = components::card_enum::SCISSORLING;
		}
		else if (val == "spider_egg")
		{
			gp.piece_type = components::card_enum::SCISSORLING_EGG;
		}
		else if (val == "spider_twin")
		{
			gp.piece_type = components::card_enum::SCISSORLING_TWIN;
		}
	}
}

component_bitset components::game_piece_loader::components_to_load()
{
	return components::game_piece::archetype_bit;
}