#ifndef EVENT_EVENT_MANAGER_HPP
#define EVENT_EVENT_MANAGER_HPP

#include "event.hpp"
#include <vector>



/*******
* Description:	A centralized system for recieving and sending events to other objects
* Usage:		Objects that inherit from listener Subscribe to event types they want to recieve
*				Objects can be unsubscribed at any time
*				Events are shared via BroadcastEvent as soon as they are recieved
* Notes:		The current implementation does not have access to timed events
*******/
class EventManager {
public:
	EventManager();
	~EventManager();

	void Subscribe(Listener* listener, EVENT_TYPE type);
	void Unsubscribe(Listener* listener, EVENT_TYPE type);
	void UnsubscribeAll(Listener* listener);

	void BroadcastEvent(Event& event);

private:

	std::vector< std::vector<Listener*> > mListeners;
};

#endif // EVENT_EVENT_MANAGER_HPP

