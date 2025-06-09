#include "pch.h"
#include "PatrolTask.h"
#include "EventManager.h"
#include "Entity.h"

void PatrolTask::update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime)
{
	if (isComplete())
		return;

	eventManager.notify<StartPatrollingEvent>(StartPatrollingEvent(entity));
	complete();
}
