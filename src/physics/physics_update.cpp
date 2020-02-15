#include "physics/physics_update.hpp"

namespace physics
{
	physics_update::physics_update(collisions::collision_manager& _c_manager, core::frame_timer& _frame_manager) :
		c_manager(_c_manager), frame_manager(_frame_manager)
	{
		contactList.reserve(512);
	}

	physics_update::~physics_update()
	{

	}

	void physics_update::update(ecs::state& state)
	{
		Step(state);
		UpdateColliders(state);
		GenerateContacts(state);

		for (int i = 0; i < contactList.size(); i++)
		{
			if (contactList[i].first_entity_id == 1 || contactList[i].first_entity_id == 18)
			{
				ecs::entity me = state.find_entity(contactList[i].first_entity_id);
				transforms::transform& t = me.get_component<transforms::transform>();
				collisions::AABB_collider& c = me.get_component<collisions::AABB_collider>();
				collisions::rigid_body& r = me.get_component<collisions::rigid_body>();
				t.position.y = t.prev_position.y;
				r.velocity.y = 0.f;
			}
			if (contactList[i].second_entity_id == 1 || contactList[i].second_entity_id == 18)
			{
				ecs::entity me = state.find_entity(contactList[i].second_entity_id);
				transforms::transform& t = me.get_component<transforms::transform>();
				collisions::AABB_collider& c = me.get_component<collisions::AABB_collider>();
				collisions::rigid_body& r = me.get_component<collisions::rigid_body>();
				t.position.y = t.prev_position.y;
				r.velocity.y = 0.f;
			}
		}

		contactList.clear();
	}

	void physics_update::GenerateContacts(ecs::state& state)
	{
		DoColliderPair<collisions::AABB_collider, collisions::AABB_collider>(state);
		DoColliderPair<collisions::AABB_collider, collisions::sphere_collider>(state);
		DoColliderPair<collisions::sphere_collider, collisions::sphere_collider>(state);
	}



	void physics_update::UpdateColliders(ecs::state& state)
	{
		state.each<transforms::transform, collisions::AABB_collider>([&](transforms::transform& transform, collisions::AABB_collider& collider)
			{
				collider.position_absolute = transform.position + collider.position_relative;
				collider.front = 0.5f * transform.scale.z;
				collider.back = 0.5f * transform.scale.z;
				collider.left = 0.5f * transform.scale.x;
				collider.right = 0.5f * transform.scale.x;
				collider.top = 0.5f * transform.scale.y;
				collider.bottom = 0.5 * transform.scale.y;
			});

		state.each< transforms::transform, collisions::sphere_collider>([&](transforms::transform& transform, collisions::sphere_collider& collider)
			{
				collider.position_absolute = transform.position + collider.position_relative;
				collider.radius = 0.5f * transform.scale.x;
			});
	}

	void physics_update::Step(ecs::state& state)
	{
		state.each<transforms::transform, collisions::rigid_body>([&](transforms::transform& transform, collisions::rigid_body& rigid_body)
			{
				transform.prev_position = transform.position;

				rigid_body.forces.y += GRAVITY * rigid_body.grav_muliplier;
				rigid_body.acceleration = rigid_body.forces * rigid_body.inv_mass;
				rigid_body.forces = glm::vec3(0);

				rigid_body.velocity *= rigid_body.lul_friction;

				rigid_body.velocity += rigid_body.acceleration * frame_manager.smoothed_delta_secs();
				transform.position += rigid_body.velocity * frame_manager.smoothed_delta_secs();

			});
	}
}