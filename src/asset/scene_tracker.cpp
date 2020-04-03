#include <asset/scene_tracker.hpp>

#include <asset/scene_change_event.hpp>

asset::scene_tracker::scene_tracker(
	std::string first_scene,
	event::EventManager& events)
	: _next(first_scene)
	, _events(events)
{
	_events.Subscribe(this, event::EVENT_TYPE::SCENE_CHANGE);
	_events.Subscribe(this, event::EVENT_TYPE::EXIT);

	
}

asset::scene_tracker::~scene_tracker()
{
	_events.Unsubscribe(this, event::EVENT_TYPE::SCENE_CHANGE);
	_events.Unsubscribe(this, event::EVENT_TYPE::EXIT);
}

std::string asset::scene_tracker::next()
{
	auto str = *_next;
	_next.reset();
	return str;
}


void asset::scene_tracker::HandleEvent(event::Event& event)
{
	switch (event.mType)
	{
	case event::EVENT_TYPE::SCENE_CHANGE:
	{
		auto& as_scene_change = (scene_change_event&)event;
		on_scene_change(as_scene_change.mNewScene);
		break;
	}
	case event::EVENT_TYPE::EXIT:
	{
		on_exit();
		break;
	}
	}
}

void asset::scene_tracker::on_exit()
{
	_next.reset();
}

void asset::scene_tracker::on_scene_change(const std::string& new_scene)
{
	_next = new_scene;
}