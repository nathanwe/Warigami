#ifndef __TO_SPAWN_HPP_
#define __TO_SPAWN_HPP_

#include "card_enum.hpp"

struct to_spawn
{
	to_spawn(int _x, int _y, int _team, components::card_enum _type) : x(_x), y(_y), team(_team), type(_type) {}
	int x;
	int y;
	int team;
	components::card_enum type;
};


#endif