#ifndef WIZARDPEOPLE_TRANSFORMS_TRANSFORMER_HPP
#define WIZARDPEOPLE_TRANSFORMS_TRANSFORMER_HPP

#include "ecs/state.hpp"
#include "ecs/system_base.hpp"
#include "transforms/transform.hpp"
#include <asset/scene_hydrater.hpp>

#include "glm/glm.hpp"

namespace transforms
{
	class transformer : public ecs::system_base
	{
	public:
		transformer(asset::scene_hydrater& hydrater);

		void update(ecs::state& state) override;

	private:
		asset::scene_hydrater& _hydrater;

		void clean_transform_hierarchy(ecs::state& r_state, entity_id id, transform& r_transform);
		bool remove_if_orphaned(ecs::state& state, entity_id id, transform& r_transform);
	};
}

#endif
