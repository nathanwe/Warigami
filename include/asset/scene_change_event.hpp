#ifndef __SCENE_CHANGE_HPP_
#define __SCENE_CHANGE_HPP_

#include <string>
#include <event/event.hpp>

namespace asset
{

	class scene_change_event : public event::Event
	{
	public:
		scene_change_event(std::string scene) 
			: event::Event(event::EVENT_TYPE::SCENE_CHANGE)
			, mNewScene(scene)
		{			
		}

		
		std::string mNewScene;
	};

}

#endif