#ifndef GAME_COMBAT_HPP
#define GAME_COMBAT_HPP

#include <ecs/state.hpp>

namespace components 
{
	class game_piece;
}

namespace combats
{
	enum class COMBAT_EFFECTS
	{
		DEAL_DAMAGE = 0,
		SPAWN_SCISSORLING_ON_DEATH = 1,
		SPAWN_SCISSOR_TROOPER_ON_DEATH = 2,
		SPAWN_SCISSORLING_FOR_HEALTH = 3,
		SLOW_TARGET = 4,
		INFEST = 5,
		SPAWN_ENEMY_SPIDERLING_ON_DEATH = 6,
		FLASH_STEP = 7,
		CREATE_FIRE_TERRAIN_ON_DEATH = 8,
		REGENERATE = 9,
		NUM
	};

	struct combat
	{
		combat(	components::game_piece& one, entity_id i_one, components::game_piece& two, entity_id i_two ) : 
				unit_one(one), id_one(i_one), unit_two(two), id_two(i_two), retaliation(false) {}
		components::game_piece& unit_one;
		entity_id id_one;
		components::game_piece& unit_two;
		entity_id id_two;
		bool retaliation;
	};

	class combat_resolution
	{
	public:
		
		std::vector<combat>& Get_Combats() { return current_combats; }
		void Add_Combat(components::game_piece& one, entity_id id_one, components::game_piece& two, entity_id id_two);
		void Resolve_Combats();

	private:
		std::vector<combat> current_combats;
	};
}
#endif