#include "pch.h"
#include "WaitUntilIdleTask.h"
#include "Entity.h"
#include "EventManager.h"

void WaitUntilIdleTask::update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime)
{
	if (isComplete())
		return;

	if (entity.getComponent<EntityStateComponent>().cCurrentState == EntityState::Idle)
		complete();
}
