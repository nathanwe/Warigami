#ifndef EVENT_EVENT_MANAGER_CPP
#define EVENT_EVENT_MANAGER_CPP

#include "event\event_manager.hpp"

/*******
* Description:	Basic constructor, sizes the listeners vector to match the number of events in the system
* Usage:		
* Notes:		
*******/
EventManager::EventManager() {

	mListeners.resize(static_cast<unsigned int>(EVENT_TYPE::NUM));
}

/*******
* Description:	Basic destructor, does nothing special
* Usage:
* Notes:
*******/
EventManager::~EventManager() {
	
}

/*******
* Description:	Used to subscribe objects to events. Objects subscribed to an event type will have their HandleEvent function called
* Usage:		First parameter must be a valid listener-inherited object, second parameter is a valid enum-class type of event
* Notes:		This function is safe from double subscribing to the same event
*******/
void EventManager::Subscribe(Listener* listener, EVENT_TYPE type) {

	auto finder = std::find(mListeners.at(static_cast<unsigned int>(type)).begin(), mListeners.at(static_cast<unsigned int>(type)).end(), listener);

	if (finder == mListeners.at(static_cast<unsigned int>(type)).end()) {
		mListeners.at(static_cast<unsigned int>(type)).push_back(listener);
	} else {
		// Listener already subscribed
	}
}

/*******
* Description:	Used to unsubscribe an object from an event type
* Usage:		First parameter must be a listener-inherited object that is subscribed, second parameter is a valid enum-class type of event
* Notes:		This function is safe from double unsubscribing from the same event
*******/
void EventManager::Unsubscribe(Listener* listener, EVENT_TYPE type) {
	
	auto finder = std::find(mListeners.at(static_cast<unsigned int>(type)).begin(), mListeners.at(static_cast<unsigned int>(type)).end(), listener);

	if (finder != mListeners.at(static_cast<unsigned int>(type)).end()) {
		std::swap(*finder, mListeners.at(static_cast<unsigned int>(type)).back());
		mListeners.at(static_cast<unsigned int>(type)).pop_back();
	} else {
		// No subscribed listener exists
	}

}

/*******
* Description:	Used to unsubscribe an object from all event types simultaneously. Best called before an object is deleted
* Usage:		First parameter must be a listener-inherited object that is subscribed
* Notes:		Call this before deleting an object
*******/
void EventManager::UnsubscribeAll(Listener* listener) {
	
	for (int i = 0; i < mListeners.size(); i++) {
		
		auto finder = std::find(mListeners.at(i).begin(), mListeners.at(i).end(), listener);

		if (finder != mListeners.at(i).end()) {
			std::swap(*finder, mListeners.at(i).back());
			mListeners.at(i).pop_back();
		}
	}
}

/*******
* Description:	Recieves an event from any source and broadcasts it to all subscribed listeners
* Usage:		First parameter is an event reference
* Notes:		This function is for instant events, not timed ones
*******/
void EventManager::BroadcastEvent(Event& event) {
	for (auto pListener : mListeners.at(static_cast<unsigned int>(event.mType))) {
		pListener->HandleEvent(event);
	}
}

#endif // EVENT_EVENT_MANAGER_CPP
