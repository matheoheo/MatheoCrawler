#include "pch.h"
#include "CastSpellTask.h"
#include "RayBehavior.h"

CastSpellTask::CastSpellTask(const SpellRule& rule, Entity& target)
	:mRule(rule),
	mTarget(target)
{
}

void CastSpellTask::update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime)
{
	if (isComplete())
		return;
	if(mRule.condition(entity, mTarget))
		mRule.execute(entity, mTarget);
	complete();
}
