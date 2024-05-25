#pragma once
#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <Windows.h>

#include <vector>

// Define priority values
#define PRIORITY_LOWEST		-2
#define PRIORITY_LOWER		-1
#define PRIORITY_NORMAL		 0
#define PRIORITY_HIGHER		 1
#define PRIORITY_HIGHEST	 2


namespace EventHandler
{
	typedef LRESULT(*EVENTFCT)(_In_ HWND, _In_ WPARAM, _In_ LPARAM, _Inout_opt_ void* context);
	typedef unsigned long long EVENTID;
	typedef short PRIORITY;

	/**
	 * @note Higher priority means being called sooner.
	 */
	struct Event
	{
		EVENTFCT eventFunction;
		void* context = nullptr;
		PRIORITY priority;
		EVENTID eventId;
	};

	typedef std::vector<Event> EventList;

	/**
	 * @brief Register an event in the event handler.
	 *
	 *
	 * @param[in] uMsg		The message at which the event function will be called.
	 * @param[in] eventFct	The function that will be called once recieving the uMsg.
	 * @param[in] priority	The higher the priority is, the later the event function will be called.
	 * @param[in] context	The context which the event function may have access to.
	 *
	 * @retval EVENTID
	 * @return The added event's id.
	 */
	EVENTID registerEvent(_In_ UINT uMsg, _In_ const EVENTFCT& eventFct, _In_opt_ void* context = nullptr, _In_opt_ PRIORITY priority = PRIORITY_NORMAL);

	/**
	 * @brief Unregister an event from the event hanlder.
	 *
	 * @note If two events have the same priority value, the older will be called first.
	 *
	 * @param[in] eventId The event's id to be removed.
	 *
	 * @retval bool
	 * @return True if the event has been remove, false otherwise.
	 */
	bool unregisterEvent(_In_ EVENTID eventId);

	/**
	 * @brief Handle a recieved message by calling all event linked to the message.
	 *
	 * @note CM_DEFAULTMESSAGE is called each time a message is retreived.
	 * 
	 * @param[in] hwnd		The hwnd related to the incoming message.
	 * @param[in] uMsg		The message to handle.
	 * @param[in] wParam	Additionnal 32bits params.
	 * @param[in] lParam	Additionnal 64bits params.
	 *
	 * @retval LRESULT
	 * @return always return S_OK.
	 *
	 * @TODO: The return value does not mean anything.
	 */
	LRESULT handleMessage(_In_ HWND hwnd, _In_ UINT uMsg, _In_opt_ WPARAM wParam, _In_opt_ LPARAM lParam);

} // namespace EventHandler

#endif // EVENTHANDLER_H