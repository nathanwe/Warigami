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
			case::combats::COMBAT_EFFECTS::INFEST:
			{
				bool infested = false;
				for (auto& effector : instance.unit_two.effects)
				{
					if (effector == combats::COMBAT_EFFECTS::SPAWN_ENEMY_SPIDERLING_ON_DEATH 
						|| effector == combats::COMBAT_EFFECTS::SPAWN_SCISSORLING_ON_DEATH 
						|| effector == combats::COMBAT_EFFECTS::SPAWN_SCISSOR_TROOPER_ON_DEATH)
					{
						infested = true;
					}
				}
				if (!infested)
				{
					instance.unit_two.effects.push_back(combats::COMBAT_EFFECTS::SPAWN_ENEMY_SPIDERLING_ON_DEATH);
				}
				break;
			}
			case::combats::COMBAT_EFFECTS::FLASH_STEP:
			{
				if (instance.retaliation && instance.unit_one.board_source.x > 0)
				{
					instance.unit_one.board_source.x--;
				}
				break;
			}
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
					for (auto& effect2 : instance.unit_one.effects)
					{
						if (effect2 == combats::COMBAT_EFFECTS::FLASH_STEP) {
							instance.unit_one.board_source.x--;
							if (instance.unit_one.board_source.x < 0) {
								instance.unit_one.board_source.x = 0;
							}
						}
					}
					break;
				case::combats::COMBAT_EFFECTS::SLOW_TARGET:
				{
					instance.unit_one.speed = instance.unit_one.speed > 1 ? instance.unit_one.speed - 1 : instance.unit_one.speed;
					break;
				}
				case::combats::COMBAT_EFFECTS::INFEST:
				{
					bool infested = false;
					for (auto& effector : instance.unit_one.effects)
					{
						if (effector == combats::COMBAT_EFFECTS::SPAWN_ENEMY_SPIDERLING_ON_DEATH
							|| effector == combats::COMBAT_EFFECTS::SPAWN_SCISSORLING_ON_DEATH
							|| effector == combats::COMBAT_EFFECTS::SPAWN_SCISSOR_TROOPER_ON_DEATH)
						{
							infested = true;
						}
					}
					if (!infested)
					{
						instance.unit_one.effects.push_back(combats::COMBAT_EFFECTS::SPAWN_ENEMY_SPIDERLING_ON_DEATH);
					}
					break;
				}
				case::combats::COMBAT_EFFECTS::FLASH_STEP:
				{
					if (instance.unit_two.board_source.x > 0)
					{
						instance.unit_two.board_source.x--;
					}
					break;
				}
				}
			}
		}
	}

	current_combats.clear();
}

void combats::combat_resolution::Add_Combat(components::game_piece& one, entity_id id_one, components::game_piece& two, entity_id id_two) 
{ 
	combats::combat new_combat(one, id_one, two, id_two);
	current_combats.push_back(new_combat); 
}