#include "pch.h"
#include "IVisualEffect.h"
#include "Entity.h"

IVisualEffect::IVisualEffect(const Entity& entity, int duration)
	:mEntity(entity),
	mEntityID(entity.getEntityId()),
	mDuration(duration),
	mTimer(0)
{
}

bool IVisualEffect::isFinished()
{
	return hasEffectDurationPassed();
}

unsigned int IVisualEffect::getEntityId() const
{
	return mEntityID;
}

void IVisualEffect::onEffectFinish()
{
}

void IVisualEffect::addTimePassed(const sf::Time& deltaTime)
{
	mTimer += deltaTime.asMilliseconds();
}

bool IVisualEffect::hasEffectDurationPassed() const
{
	return mTimer >= mDuration;
}
