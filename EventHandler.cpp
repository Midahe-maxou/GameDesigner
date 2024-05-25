#include "EventHandler.h"

#include <Windows.h>

#include <unordered_map>
#include <vector>

#include "fctdef.h"


namespace EventHandler
{

	std::unordered_map<UINT, std::vector<Event>> eventHandler;
	EVENTID lastEventId = 0xf; // First 15 IDs are reserved.


	void addEvent(_In_ Event&& ev, _Out_ EventList* eventList) noexcept
	{
		size_t eventIndice = 0;
		if (eventList->size() != 0)
			// Find by dichotomous search the emplacement where to put the new event based on priority.
			dichotomous_search<Event, PRIORITY>(*eventList, ev.priority, [](Event e) -> PRIORITY { return e.priority; });

		EventList::const_iterator it = eventList->begin() + eventIndice; // Translate size_t to iterator.
		eventList->insert(it, std::move(ev));
	}

	bool removeEvent(_In_ EVENTID eventId, _Out_ EventList* eventList) noexcept
	{
		for (auto it = eventList->begin(); it < eventList->end(); it++)
		{
			if (it->eventId == eventId)
			{
				eventList->erase(it);
				return true;
			}
		}
		return false;
	}
	

	bool removeEvent(_In_ Event&& e, _Out_ EventList* eventList)
	{
		return removeEvent(e.eventId, eventList);
	}

	EVENTID registerEvent(_In_ UINT uMsg, _In_ const EVENTFCT& eventFct, _In_opt_ void* context, _In_opt_ PRIORITY priority)
	{
		EVENTID id = (lastEventId += 1);
		Event ev{ eventFct, context, priority, id };

		EventList eventList;

		// Retrieve previous events.
		if (auto it = eventHandler.find(uMsg); it != eventHandler.end())
			eventList = it->second;

		addEvent(std::move(ev), &eventList);
		eventHandler[uMsg] = eventList;
		return id;
	}

	bool unregisterEvent(_In_ EVENTID eventId)
	{
		for (std::pair<UINT, std::vector<Event>> listEvents : eventHandler)
		{
			EventList eventList = listEvents.second;
			if (removeEvent(eventId, &eventList))
				return true;
		}
		return false;
	}

	LRESULT handleMessage(_In_ HWND hwnd, _In_ UINT uMsg, _In_opt_ WPARAM wParam, _In_opt_ LPARAM lParam)
	{
		auto it = eventHandler.find(uMsg);
		if (it == eventHandler.end())
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		std::vector<Event> events = it->second; // Get the event from the std::pair.

		for (auto ev = events.rbegin(); ev < events.rend(); ev++)
		{
			EVENTFCT winProc = ev->eventFunction;
			(*winProc)(hwnd, wParam, lParam, ev->context);
		}
		return S_OK;
	}
} // namespace EventHandler

