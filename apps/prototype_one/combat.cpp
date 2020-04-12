#include "combat.hpp"

#include "rendering/renderable_mesh_static.hpp"

#include "components/game_piece.hpp"

void combats::combat_resolution::Resolve_Combats()
{
	for (auto& instance : current_combats)
	{
		bool unit_two_flash_step = false;
		bool unit_two_protection_from_arrows = false;
		bool unit_two_speed_enrage = false;
		bool unit_one_speed_enrage = false;
		bool unit_two_only_one_damage = false;
		bool unit_one_only_one_damage = false;
		for (auto& effect2 : instance.unit_two.effects)
		{
			if (effect2 == combats::COMBAT_EFFECTS::FLASH_STEP) {
				unit_two_flash_step = true;
			}
			if (effect2 == combats::COMBAT_EFFECTS::PROTECTION_FROM_ARROWS) {
				unit_two_protection_from_arrows = true;
			}
			if (effect2 == combats::COMBAT_EFFECTS::SPEED_ENRAGE) {
				unit_two_speed_enrage = true;
			}
			if (effect2 == combats::COMBAT_EFFECTS::ONLY_ONE_DAMAGE) {
				unit_two_only_one_damage = true;
			}
		}
		for (auto& effect : instance.unit_one.effects)
		{
			switch (effect)
			{
			case combats::COMBAT_EFFECTS::DEAL_DAMAGE:
			{
				if (!(!instance.retaliation && unit_two_protection_from_arrows)) {
					int damage = unit_two_only_one_damage ? 1 : instance.unit_one.damage;
					for (int i = 0; i < damage; i++) {
						if (instance.unit_two.health > 0) {
							instance.unit_two.health--;
							// Turn health spheres red
							if (instance.unit_two.health_points.size() >= instance.unit_two.health) {
								instance.unit_two.health_points[instance.unit_two.health].get_component<rendering::renderable_mesh_static>().material.param_diffuse = glm::vec3(1, 0, 0);
							}
						}
					}

					if (!instance.retaliation && unit_two_flash_step) {
						instance.unit_two.flash_step_flag = true;
					}
					if (unit_two_speed_enrage) {
						instance.unit_two.speed += 9;
					}
				}
				break;
			}
			case::combats::COMBAT_EFFECTS::SLOW_TARGET:
			{
				instance.unit_two.speed = instance.unit_two.speed > 1 ? instance.unit_two.speed - 1 : instance.unit_two.speed;
				break;
			}
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
			case::combats::COMBAT_EFFECTS::ANTI_REGEN:
			{
				instance.unit_two.effects.erase(std::remove(instance.unit_two.effects.begin(),
					instance.unit_two.effects.end(), COMBAT_EFFECTS::REGENERATE), instance.unit_two.effects.end());
				break;
			}
			case::combats::COMBAT_EFFECTS::SPEED_ENRAGE:
			{
				unit_one_speed_enrage = true;
				break;
			}
			case::combats::COMBAT_EFFECTS::ONLY_ONE_DAMAGE	:
			{
				unit_one_only_one_damage = true;
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
				{
					int damage = unit_one_only_one_damage ? 1 : instance.unit_two.damage;
					for (int i = 0; i < damage; i++) {
						if (instance.unit_one.health > 0) {
							instance.unit_one.health--;
							// Turn health spheres red
							if (instance.unit_one.health_points.size() >= instance.unit_one.health) {
								instance.unit_one.health_points[instance.unit_one.health].get_component<rendering::renderable_mesh_static>().material.param_diffuse = glm::vec3(1, 0, 0);
							}
						}
					}
					if (unit_one_speed_enrage) {
						instance.unit_one.speed += 9;
					}
					break;
				}
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
				case::combats::COMBAT_EFFECTS::ANTI_REGEN:
				{
					instance.unit_one.effects.erase(std::remove(instance.unit_one.effects.begin(),
						instance.unit_one.effects.end(), COMBAT_EFFECTS::REGENERATE), instance.unit_one.effects.end());
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
