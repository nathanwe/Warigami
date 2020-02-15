#ifndef PHYSICS_PHYSICS_UPDATE_HPP
#define PHYSICS_PHYSICS_UPDATE_HPP

#define GRAVITY 10.0f

#include "ecs/state.hpp"
#include "ecs/system_base.hpp"
#include "collisions/collision_manager.hpp"
#include "core/frame_timer.hpp"
#include "transforms/transform.hpp"

namespace physics
{

	struct contact
	{
		contact(entity_id first, entity_id second) : first_entity_id(first), second_entity_id(second){}
		entity_id first_entity_id;
		entity_id second_entity_id;
	};

	class physics_update : public ecs::system_base
	{
	public:
		physics_update(collisions::collision_manager& _c_manager, core::frame_timer& _frame_manager);
		~physics_update();
		virtual void update(ecs::state& state) override;
		void GenerateContacts(ecs::state& state);
		void UpdateColliders(ecs::state& state);
		void Step(ecs::state& state);
	public:
		collisions::collision_manager& c_manager;
		core::frame_timer& frame_manager;
		std::vector<contact> contactList;
	};
}

#endif