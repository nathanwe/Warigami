#ifndef __SCENE_CONTEXT_HPP_
#define __SCENE_CONTEXT_HPP_

#include <optional>
#include <string>
#include <event/event.hpp>
#include <event/event_manager.hpp>

namespace asset
{

	class scene_tracker : public event::Listener
	{
	public:
		explicit scene_tracker(std::string first_scene, event::EventManager& events);
		~scene_tracker();

		bool has_next() const { return _next.has_value(); }

		std::string next();

		void HandleEvent(event::Event& event) override;


	private:
		std::optional<std::string> _next;

		event::EventManager& _events;

		void on_exit();

		void on_scene_change(const std::string& new_scene);
	};

}

#endif
