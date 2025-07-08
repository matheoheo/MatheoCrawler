#include "pch.h"
#include "PositionToAttackTask.h"
#include "EventManager.h"

PositionToAttackTask::PositionToAttackTask(Entity& target)
	:mTarget(target)
{
}

void PositionToAttackTask::update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime)
{
	if (isComplete())
		return;

	eventManager.notify<RepositionToAttackEvent>(RepositionToAttackEvent(entity, mTarget));
	complete();
}
