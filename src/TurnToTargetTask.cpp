#include "pch.h"
#include "TurnToTargetTask.h"
#include "IBehavior.h"

TurnToTargetTask::TurnToTargetTask(Entity& target, IBehavior* behavior)
	:mTarget(target),
	mBehavior(behavior)
{
}

void TurnToTargetTask::update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime)
{
	if (isComplete())
		return;

	if (mBehavior)
		mBehavior->setDirectionTowardTarget(entity, mTarget);

	complete();
}
