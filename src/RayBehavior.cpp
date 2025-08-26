#include "pch.h"
#include "RayBehavior.h"
#include "Entity.h"
#include "Utilities.h"
#include "EntityManager.h"
#include "Random.h"

RayBehavior::RayBehavior(BehaviorContext& behaviorContext)
	:IBehavior(behaviorContext)
{
}

void RayBehavior::update(Entity& entity, const sf::Time& deltaTime)
{
	updateTimers(entity, deltaTime);
	updateTasks(entity, deltaTime);
}

void RayBehavior::determineNextTask(Entity& entity)
{
	auto& target = mBehaviorContext.entityManager.getPlayer();
}

void RayBehavior::swapToTargetting(Entity& entity)
{
}

void RayBehavior::swapToAttacking(Entity& entity, Entity& target)
{
}

void RayBehavior::fallbackOnNoDirection(Entity& self, Entity& target)
{
}

void RayBehavior::handleTargettingLogic(Entity& self, Entity& target)
{
}

void RayBehavior::handleAttackingLogic(Entity& self, Entity& target)
{
}

const RayBehavior::PhaseConfig& RayBehavior::getPhase(const Entity& entity)
{
	const auto& statsComp = entity.getComponent<CombatStatsComponent>();
	int currHp = statsComp.cHealth;
	int maxHp = statsComp.cMaxHealth;
	int percentage = static_cast<int>((static_cast<float>(currHp) / static_cast<float>(maxHp)) * 100);

	for (const auto& phase : RayBehaviorConfig::phases)
	{
		if (percentage >= phase.hpThreshold)
			return phase;
	}

	return RayBehaviorConfig::phases[RayBehaviorConfig::MaxPhases - 1];
}

bool RayBehavior::canCastSpell(const Entity& entity, SpellIdentifier id)
{
	return Utilities::hasSpellCdPassed(entity, id);
}

AnimationIdentifier RayBehavior::getAttack(const Entity& entity)
{
	const auto& phase = getPhase(entity);
	const auto& attacks = phase.unlockedAttacks;
	if (attacks.size() == 1)
		return attacks[0];
	if (attacks.size() > 1)
		return attacks[Random::get(0, attacks.size() - 1)];

	return AnimationIdentifier::Attack1;
}
