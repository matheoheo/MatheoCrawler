#include "pch.h"
#include "DelayTask.h"

DelayTask::DelayTask(int interval)
	:mDelayInterval(interval)
{
}

void DelayTask::update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime)
{
	mDelayInterval -= deltaTime.asMilliseconds();

	if (mDelayInterval <= 0)
		complete();
}
