#include "pch.h"
#include "EventManager.h"

EventManager::EventManager() :mEventId(0)
{
}

void EventManager::unregister(const EventHandle& eventHandle)
{
	if (!mCallbacks.contains(eventHandle.typeIndex))
		return;

	auto& vec = mCallbacks.at(eventHandle.typeIndex);
	vec.erase(std::remove_if(std::begin(vec), std::end(vec),
		[&](const auto& pair)
		{
			return pair.first == eventHandle.index;
		}), std::end(vec));

}
