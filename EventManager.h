#pragma once
#include "GameEvents.h"
#include "ConceptDefinitions.h"
#include <typeindex>
#include <functional>

class EventManager
{
public:
	struct EventHandle
	{
		std::type_index typeIndex;
		std::size_t index;
	};
	EventManager();
	template <GameEventType Event>
	EventHandle registerEvent(std::function<void(const Event&)> callback);

	template <GameEventType Event>
	void notify(const Event& ev);

	void unregister(const EventHandle& eventHandle);
private:
	std::unordered_map<std::type_index, std::vector<std::pair<size_t, std::function<void(const IEvent&)>>>> mCallbacks;
	size_t mEventId;
};

template<GameEventType Event>
inline EventManager::EventHandle EventManager::registerEvent(std::function<void(const Event&)> callback)
{
	auto wrapper = [cb = std::move(callback)](const IEvent& e)
	{
		cb(static_cast<const Event&>(e));
	};
	auto [it, inserted] = mCallbacks.try_emplace(typeid(Event));

	++mEventId;
	it->second.emplace_back(mEventId, std::move(wrapper));
	return { typeid(Event), mEventId };
}

template<GameEventType Event>
inline void EventManager::notify(const Event& ev)
{
	if (!mCallbacks.contains(typeid(Event)))
		return;

	for (const auto& callbackPair : mCallbacks.at(typeid(Event)))
	{
		callbackPair.second(ev);
	}
}

