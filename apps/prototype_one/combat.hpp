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
		DEAL_DAMAGE,
		SPAWN_SCISSORLING_ON_DEATH,
		SPAWN_SCISSOR_TROOPER_ON_DEATH,
		SPAWN_SCISSORLING_SELF_TARGET,
		SLOW_TARGET,
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