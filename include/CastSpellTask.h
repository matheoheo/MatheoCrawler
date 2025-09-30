#pragma once
#include "ITask.h"

struct SpellRule;

class CastSpellTask
	:public ITask
{
public:
	CastSpellTask(const SpellRule& rule, Entity& target);
	// Inherited via ITask
	virtual void update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime) override;
private:
	const SpellRule& mRule;
	Entity& mTarget;
};