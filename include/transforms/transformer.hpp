#ifndef WIZARDPEOPLE_TRANSFORMS_TRANSFORMER_HPP
#define WIZARDPEOPLE_TRANSFORMS_TRANSFORMER_HPP

#include "ecs/state.hpp"
#include "ecs/system_base.hpp"
#include "transforms/transform.hpp"

#include "glm/glm.hpp"

namespace transforms
{
	class transformer : public ecs::system_base
	{
	public:
		virtual void update(ecs::state& state) override;

	private:
		bool clean_transform_hierarchy(ecs::state& r_state, entity_id id, transform& r_transform);
	};
}

#endif
