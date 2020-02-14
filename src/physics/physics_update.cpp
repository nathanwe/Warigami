#include "physics/physics_update.hpp"

namespace physics
{
	physics_update::physics_update(collisions::collision_manager& _c_manager) : c_manager(_c_manager)
	{
		contactList.reserve(512);
	}

	physics_update::~physics_update() 
	{

	}

	void physics_update::update(ecs::state& state)
	{
		UpdateColliders(state);
		GenerateContacts(state);

		for (int i = 0; i < contactList.size(); i++)
		{
			if (contactList[i].first_entity_id == 1)
			{
				ecs::entity me = state.find_entity(contactList[i].first_entity_id);
				transforms::transform& t = me.get_component<transforms::transform>();
				t.position.x = 0.f;
				t.position.y = 0.f;
				t.position.z = 0.f;
			}
			if (contactList[i].second_entity_id == 1)
			{
				ecs::entity me = state.find_entity(contactList[i].second_entity_id);
				transforms::transform& t = me.get_component<transforms::transform>();
				t.position.x = 0.f;
				t.position.y = 0.f;
				t.position.z = 0.f;
			}
		}

		contactList.clear();
	}

	void physics_update::GenerateContacts(ecs::state& state)
	{
		state.each<collisions::AABB_collider, collisions::rigid_body>([&](collisions::AABB_collider& collider_one, collisions::rigid_body& useless)
		{
			state.each<collisions::AABB_collider>([&](collisions::AABB_collider& collider_two)
			{
				if (collider_one.owner_id != collider_two.owner_id)
				{
					if (c_manager.check_collision(&collider_one, &collider_two))
					{
						contactList.emplace_back(collider_one.owner_id, collider_two.owner_id);
						std::cerr << "Contact registered!" << std::endl;
					}
				}
			});
		});
		
		state.each<collisions::sphere_collider, collisions::rigid_body>([&](collisions::sphere_collider& collider_one, collisions::rigid_body& useless)
		{
			state.each<collisions::sphere_collider>([&](collisions::sphere_collider& collider_two)
			{
				if (collider_one.owner_id != collider_two.owner_id)
				{
					if (c_manager.check_collision(&collider_one, &collider_two))
					{
						contactList.emplace_back(collider_one.owner_id, collider_two.owner_id);
					}
				}
			});
		});

		state.each<collisions::AABB_collider, collisions::rigid_body>([&](collisions::AABB_collider& collider_one, collisions::rigid_body& useless)
		{
			state.each<collisions::sphere_collider>([&](collisions::sphere_collider& collider_two)
			{
				if (collider_one.owner_id != collider_two.owner_id)
				{
					if (c_manager.check_collision(&collider_one, &collider_two))
					{
						contactList.emplace_back(collider_one.owner_id, collider_two.owner_id);
					}
				}
			});
		});
	}

	void physics_update::UpdateColliders(ecs::state& state)
	{
		state.each< transforms::transform, collisions::AABB_collider>([&](transforms::transform& transform, collisions::AABB_collider& collider)
		{
			collider.position_absolute.x = transform.position.x + collider.position_relative.x;
			collider.position_absolute.y = transform.position.y + collider.position_relative.y;
			collider.position_absolute.z = transform.position.z + collider.position_relative.z;
		});

		state.each< transforms::transform, collisions::sphere_collider>([&](transforms::transform& transform, collisions::sphere_collider& collider)
		{
			collider.position_absolute.x = transform.position.x + collider.position_relative.x;
			collider.position_absolute.y = transform.position.y + collider.position_relative.y;
			collider.position_absolute.z = transform.position.z + collider.position_relative.z;
		});
	}
}
