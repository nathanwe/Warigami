#ifndef EVENT_EVENT_HPP
#define EVENT_EVENT_HPP

namespace event {

	/*******
	* Description:	List of unique identifiers for all event types
	* Usage:		Add a corresponding unique identifier to list list for all unique events in the system
	* Notes:		Keep 'Num' at the end of the list, it can be cast to an integer to count the size of the list
	*******/
	enum class EVENT_TYPE {
		EXPLOSION, // Example event
		CONSOLE_COMMAND,
		TOGGLE_NOCLIP,
		UNIT_SPAWN,
		UNIT_DEATH,
		GAME_OVER,
		GAME_START,
		SCENE_CHANGE,
		EXIT,
		TOGGLE_P1_AI,
		TOGGLE_P2_AI,
		ATTRACT_MODE,
		NUM
	};

	/*******
	* Description:	Parent event class for all events
	* Usage:		Inherit from this class for all events
	* Notes:		If you want to delay an event, set a timer in seconds
	*******/
	class Event {
	public:
		Event(EVENT_TYPE type) : mType(type) {}
		~Event() {}
		EVENT_TYPE mType;
	};

	/*******
	* Description:	Parent Listener class for any system that needs access to event handling
	* Usage:		Inherit from this class for any system that wants to handle events
	* Notes:		An overloaded HandleEvent function is required for any class that inherits from this
	*******/
	class Listener {
	public:
		Listener() {}
		~Listener() {}

		virtual void HandleEvent(Event& event) = 0;
	};
}
#endif // EVENT_EVENT_HPP
