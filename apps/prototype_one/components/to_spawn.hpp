#ifndef __TO_SPAWN_HPP_
#define __TO_SPAWN_HPP_

#include "card_enum.hpp"
#include "game_piece.hpp"

struct to_spawn
{
	to_spawn(int _x, int _y, int _team, components::card_enum _type) : x(_x), y(_y), team(_team), type(_type) {}
	int x;
	int y;
	int team;
	components::card_enum type;
	components::UNIT_STATE state{ components::UNIT_STATE::WAIT };
};


#endif