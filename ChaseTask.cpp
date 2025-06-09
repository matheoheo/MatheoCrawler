#include "pch.h"
#include "ChaseTask.h"
#include "EventManager.h"

ChaseTask::ChaseTask(Entity& targetEntity)
	:mTarget(targetEntity)
{
}

void ChaseTask::update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime)
{
	if (isComplete())
		return;

	eventManager.notify<StartChasingEvent>(StartChasingEvent(entity, mTarget));
	complete();
}
