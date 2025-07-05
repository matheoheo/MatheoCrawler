#include "pch.h"
#include "BasicRangedBehavior.h"

BasicRangedBehavior::BasicRangedBehavior(BehaviorContext& behaviorContext)
	:IBehavior(behaviorContext)
{
}

void BasicRangedBehavior::update(Entity& entity, const sf::Time& deltaTime)
{
}

void BasicRangedBehavior::determineNextTask(Entity& entity)
{
}
