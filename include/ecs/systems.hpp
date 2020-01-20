#ifndef __SYSTEMS_HPP_
#define __SYSTEMS_HPP_

#include <vector>

#include <ecs/system_base.hpp>

namespace ecs
{

	class systems
	{
	public:
		systems(std::vector<system_base*> systems) : _systems(systems) {}

		void update(state& state);

	private:
		std::vector<system_base*> _systems;
	};


}
#endif // !__SYSTEMS_HPP_
