#include "combat.hpp"

#include "components/game_piece.hpp"

void combats::combat_resolution::Resolve_Combats()
{
	for (auto& instance : current_combats)
	{
		for (auto& effect : instance.unit_one.effects)
		{
			switch (effect)
			{
			case combats::COMBAT_EFFECTS::DEAL_DAMAGE:
				instance.unit_two.health -= instance.unit_one.damage;
				break;
			case::combats::COMBAT_EFFECTS::SLOW_TARGET:
				instance.unit_two.speed = instance.unit_two.speed > 1 ? instance.unit_two.speed - 1 : instance.unit_two.speed;
				break;
			}
		}

		if (instance.retaliation)
		{
			for (auto& effect : instance.unit_two.effects)
			{
				switch (effect)
				{
				case combats::COMBAT_EFFECTS::DEAL_DAMAGE:
					instance.unit_one.health -= instance.unit_two.damage;
					break;
				case::combats::COMBAT_EFFECTS::SLOW_TARGET:
					instance.unit_one.speed = instance.unit_one.speed > 1 ? instance.unit_one.speed - 1 : instance.unit_one.speed;
					break;
				}
			}
		}
	}
}

void combats::combat_resolution::Clear()
{
	current_combats.clear();
}

void combats::combat_resolution::Add_Combat(components::game_piece& one, entity_id id_one, components::game_piece& two, entity_id id_two) 
{ 
	combats::combat new_combat(one, id_one, two, id_two);
	current_combats.push_back(new_combat); 
}