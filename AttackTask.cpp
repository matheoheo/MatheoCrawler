#include "pch.h"
#include "AttackTask.h"
#include "Entity.h"
#include "EventManager.h"
AttackTask::AttackTask(AnimationIdentifier animId)
	:mAnimId(animId)
{
}

void AttackTask::update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime)
{
	if (isComplete())
		return;

	eventManager.notify<StartAttackingEvent>(StartAttackingEvent(entity, mAnimId));
	complete();
}
