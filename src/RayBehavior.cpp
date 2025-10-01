#include "pch.h"
#include "RayBehavior.h"
#include "Entity.h"
#include "Utilities.h"
#include "EntityManager.h"
#include "Random.h"
#include "TileMap.h"
#include "SpellHolder.h"
#include "WaitUntilIdleTask.h"
#include "ChaseTask.h"
#include "TurnToTargetTask.h"
#include "AttackTask.h"
#include "CastSpellTask.h"

RayBehavior::RayBehavior(BehaviorContext& behaviorContext)
	:IBehavior(behaviorContext),
	mCurrentSpell(nullptr)
{
	setupPhases();
	setupBehaviors();
}

void RayBehavior::update(Entity& entity, const sf::Time& deltaTime)
{
	updateTimers(entity, deltaTime);
	updateTasks(entity, deltaTime);
}

void RayBehavior::determineNextTask(Entity& entity)
{
	auto& target = mBehaviorContext.entityManager.getPlayer();
	auto& aiStateComp = entity.getComponent<EntityAIStateComponent>();
	auto state = aiStateComp.cState;
	determineCurrentSpell(entity, target);
	if (mCurrentSpell)
	{
		swapToAttacking(entity, target);
		return;
	}
	updateCurrentBehavior(entity, target, state);
	pushDelayTask(90);

}

void RayBehavior::swapToTargetting(Entity& entity)
{
	pushTask(std::make_unique<WaitUntilIdleTask>());
	pushTask(std::make_unique<ChaseTask>(entity));
	pushDelayTask(getRandomDelay(250));
}

void RayBehavior::swapToAttacking(Entity& entity, Entity& target)
{
	entity.getComponent<EntityAIStateComponent>().cState = EntityAIState::Attacking;
	entity.getComponent<PathComponent>().cPathCells.clear();

	if (mCurrentSpell)
	{
		pushTask(std::make_unique<WaitUntilIdleTask>());
		pushTask(std::make_unique<TurnToTargetTask>(target, this));
		pushTask(std::make_unique<CastSpellTask>(*mCurrentSpell, target));
	}
	else
	{
		auto attackType = getAttack(entity);
		pushTask(std::make_unique<WaitUntilIdleTask>());
		pushTask(std::make_unique<TurnToTargetTask>(target, this));
		pushTask(std::make_unique<AttackTask>(attackType));

	}
	pushDelayTask(getRandomDelay(250));
}

void RayBehavior::fallbackOnNoDirection(Entity& self, Entity& target)
{
	swapToTargetting(target);
}

void RayBehavior::handleTargettingLogic(Entity& self, Entity& target)
{
	if (canAttack(self, target) || mCurrentSpell)
	{
		swapToAttacking(self, target);
		return;
	}

	auto& chaseComp = self.getComponent<ChaseAIComponent>();
	if (!chaseComp.cTarget)
	{
		//if there is no target to chase, then just go back to patrol?
		swapToPatrol();
		return;
	}

	bool isReachable = isEntityReachable(*chaseComp.cTarget);
	bool pathExists = mBehaviorContext.tileMap.doesPathExist(self, *chaseComp.cTarget);
	if (isReachable && pathExists)
		chaseComp.cUnreachableRetryCount = 0;

	if (!isReachable || !pathExists)
	{
		pushDelayTask(50);
		++chaseComp.cUnreachableRetryCount;
		constexpr int maxRetriesAttempts = 5;
		//if entity can't reach target, but is still within limit, then we add some delay.
		if (chaseComp.cUnreachableRetryCount <= maxRetriesAttempts)
		{
			//we give it some delay, and then he will try again.
			pushDelayTask(getRandomDelay(150));
			return;
		}
		else
		{
			//if reach tries exceeded threshold, then we just swap to patrolling.
			swapToPatrol();
			return;
		}
	}

	bool inFOVRange = isTargetInFOV(*chaseComp.cTarget, target);
	if (!inFOVRange)
	{
		//if target moved outside fov range, we just go back to patrolling
		swapToPatrol();
	}
}

void RayBehavior::handleAttackingLogic(Entity& self, Entity& target)
{
	if (canAttack(self, target))
	{
		if (self.getComponent<EntityStateComponent>().cCurrentState == EntityState::Idle)
		{
			swapToAttacking(self, target);
		}
	}
	else
	{
		swapToTargetting(target);
	}
}

void RayBehavior::determineCurrentSpell(Entity& self, Entity& target)
{
	mCurrentSpell = nullptr;
	auto& phase = getPhase(self);
	for (auto& rule : phase.spellRules)
	{
		if (canCastSpell(self, rule.spellId) && rule.condition(self, target))
		{
			mCurrentSpell = &rule;
			return;
		};
	}
}

const RayBehavior::PhaseConfig& RayBehavior::getPhase(const Entity& entity)
{
	const auto& statsComp = entity.getComponent<CombatStatsComponent>();
	int currHp = statsComp.cHealth;
	int maxHp = statsComp.cMaxHealth;
	int percentage = static_cast<int>((static_cast<float>(currHp) / static_cast<float>(maxHp)) * 100);

	for (const auto& phase : mPhases)
	{
		if (percentage >= phase.minHpPercent && percentage <= phase.maxHpPercent)
			return phase;
	}

	return mPhases.front();
}

void RayBehavior::setupPhases()
{
	constexpr size_t phasesCount = 3;
	mPhases.reserve(phasesCount);
	setupFirstPhase();
	setupSecondPhase();

	for (auto& phase : mPhases)
		sortSpellRules(phase);
}

void RayBehavior::setupFirstPhase()
{
	auto& phase = mPhases.emplace_back();
	phase.spellRules.reserve(4);
	phase.minHpPercent = 80;
	phase.maxHpPercent = 100;
	phase.unlockedAttacks.emplace_back(AnimationIdentifier::Attack1);
	phase.unlockedSpells.emplace_back(SpellIdentifier::WaterBall);
	phase.unlockedSpells.emplace_back(SpellIdentifier::PureProjectile);
	phase.unlockedSpells.emplace_back(SpellIdentifier::BladeDance);

	//phase.spellRules.push_back(createWaterBallRule());
	//phase.spellRules.push_back(createBeamRule());
	//phase.spellRules.push_back(createPureProjectileRule());
	phase.spellRules.push_back(createBladeDanceRule());
}

void RayBehavior::setupSecondPhase()
{
	auto& phase = mPhases.emplace_back();
	phase.maxHpPercent = 80;
	phase.minHpPercent = 0;

	phase.unlockedAttacks.emplace_back(AnimationIdentifier::Attack3);
	phase.unlockedSpells.emplace_back(SpellIdentifier::WaterBall);
	phase.unlockedSpells.emplace_back(SpellIdentifier::LightBeam);
	phase.spellRules.push_back(createWaterBallRule());
	phase.spellRules.push_back(createBeamRule());
}

void RayBehavior::sortSpellRules(PhaseConfig& phase)
{
	std::ranges::sort(phase.spellRules, [](const SpellRule& a, const SpellRule& b)
		{
			return a.priority > b.priority;
		});
}

SpellRule RayBehavior::createWaterBallRule()
{
	constexpr float waterballDmgPercent = 0.12f;
	SpellRule waterBallRule;
	waterBallRule.name = "Waterball";
	waterBallRule.spellId = SpellIdentifier::WaterBall;
	waterBallRule.priority = 5;
	waterBallRule.condition = [this](const Entity& self, const Entity& target) -> bool
	{
		return canCastProjectile(self, target, SpellHolder::getInstance().get(SpellIdentifier::WaterBall).projectile->range);
	};
	waterBallRule.execute = [this](Entity& self, Entity& target)
	{
		castProjectile(self, target, waterballDmgPercent, SpellIdentifier::WaterBall);
	};

	return waterBallRule;
}

SpellRule RayBehavior::createBeamRule()
{
	SpellRule rule;
	rule.name = "Beam";
	rule.priority = 8;
	rule.spellId = SpellIdentifier::LightBeam;
	rule.condition = [this](const Entity& self, const Entity& target) -> bool
	{
		return canCastBeam(self, target);
	};
	rule.execute = [this](Entity& self, Entity& target)
	{
		castBeam(self, target);
	};

	return rule;
}

SpellRule RayBehavior::createPureProjectileRule()
{
	constexpr float projDmgPercent = 0.25f;
	constexpr int axisTolerance = 1;
	SpellRule rule;
	rule.name = "Pure Projectile";
	rule.spellId = SpellIdentifier::PureProjectile;
	rule.priority = 8;
	rule.condition = [this](const Entity& self, const Entity& target) -> bool
	{
		return canCastProjectile(self, target, SpellHolder::getInstance().get(SpellIdentifier::PureProjectile).projectile->range, axisTolerance);
	};
	rule.execute = [this](Entity& self, Entity& target)
	{
		castProjectile(self, target, projDmgPercent, SpellIdentifier::PureProjectile);
	};

	return rule;
}

SpellRule RayBehavior::createBladeDanceRule()
{
	SpellRule rule;
	rule.name = "Blade Dance";
	rule.spellId = SpellIdentifier::BladeDance;
	rule.priority = 20;
	rule.condition = [this](const Entity& self, const Entity& target) -> bool
	{
		constexpr int requiredMinDist = 2;
		return getDistance(self, target) <= requiredMinDist;
	};
	rule.execute = [this](Entity& self, Entity& target)
	{
		constexpr float perTickDmgPercent = 0.07f;
		int dmgPerTick = getTargetsHealth(target, perTickDmgPercent);
		mBehaviorContext.eventManager.notify<CastSpellEvent>(CastSpellEvent(self, SpellIdentifier::BladeDance, {}, [this, &self, dmgPerTick]()
			{
				mBehaviorContext.eventManager.notify<CastAOESpellEvent>(CastAOESpellEvent(self, SpellIdentifier::BladeDance,
					Utilities::getEntityPos(self), dmgPerTick));
			}));
	};
	return rule;
}

bool RayBehavior::canCastSpell(const Entity& entity, SpellIdentifier id)
{
	return Utilities::hasSpellCdPassed(entity, id);
}

bool RayBehavior::canCastProjectile(const Entity& self, const Entity& target, int range, int axisTolerance) const
{
	bool areAligned = Utilities::areAxisAligned(self, target, axisTolerance);
	if (!areAligned || getDistance(self, target) > range)
		return false;

	bool isLineOfSightClear = mBehaviorContext.tileMap.isLineOfSightClear(Utilities::getEntityCell(self), Utilities::getEntityCell(target));
	return areAligned && isLineOfSightClear;
}

void RayBehavior::castProjectile(Entity& self, Entity& target, float dmgPercent, SpellIdentifier id)
{
	int dmg = getTargetsHealth(target, dmgPercent);
	mBehaviorContext.eventManager.notify<CastSpellEvent>(CastSpellEvent(self, id, {}, [this, &self, id, dmg]()
		{
			mBehaviorContext.eventManager.notify<SpawnProjectileEvent>(SpawnProjectileEvent(self, id, RayBehaviorConfig::rayColor, dmg));
		}));
}

bool RayBehavior::canCastBeam(const Entity& self, const Entity& target) const
{
	constexpr int minDist = 2;
	constexpr int maxDist = 5; //temporary
	constexpr int tolerance = 1;
	int dist = getDistance(self, target);
	if (dist < minDist || dist > maxDist)
		return false;

	auto selfCell = Utilities::getEntityCell(self);
	auto targetCell = Utilities::getEntityCell(target);

	int dx = std::abs(selfCell.x - targetCell.x);
	int dy = std::abs(selfCell.y - targetCell.y);
	if (dx > tolerance && dy > tolerance)
		return false;
	
	//means that entities are aligned 
	if (dx == 0 || dy == 0)
		return mBehaviorContext.tileMap.isLineOfSightClear(selfCell, targetCell);

	if (dy > dx)
		selfCell.x = targetCell.x;
	else
		selfCell.y = targetCell.y;

	return mBehaviorContext.tileMap.isLineOfSightClear(selfCell, targetCell);
}

void RayBehavior::castBeam(Entity& self, Entity& target)
{
	int damage = getTargetsHealth(target, 0.2f); //20% of target's hp
	BeamData data
	{
		.damage = damage,
		.length = 4,
		.chargeTime = 1000,
		.outerColor = {255, 200, 50, 180},
		.innerColor = RayBehaviorConfig::rayColor,
		.casterPos = Utilities::getEntityPos(self),
		.dir = self.getComponent<DirectionComponent>().cCurrentDir
	};

	mBehaviorContext.eventManager.notify<CastSpellEvent>(CastSpellEvent(self, SpellIdentifier::LightBeam, {}, [this, data]()
		{
			mBehaviorContext.eventManager.notify<CastBeamEvent>(CastBeamEvent(data));
		}));
}

int RayBehavior::getDistance(const Entity& self, const Entity& target) const
{
	const auto selfCell = Utilities::getEntityCell(self);
	const auto targetCell = Utilities::getEntityCell(target);
	const int dx = std::abs(selfCell.x - targetCell.x);
	const int dy = std::abs(selfCell.y - targetCell.y);

	return std::max(dx, dy);
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

void RayBehavior::setupBehaviors()
{
	setupCommonBehaviors();
	mBehaviors[EntityAIState::Chasing] = [this](Entity& self, Entity& target)
	{
		handleTargettingLogic(self, target);
	};
}

bool RayBehavior::isInMeleeRange(const Entity& self, const Entity& target) const
{
	return getDistance(self, target) == 1;
}

int RayBehavior::getTargetsHealth(const Entity& target, float percent) const
{
	auto targetHp = target.getComponent<CombatStatsComponent>().cMaxHealth;
	int hp = static_cast<int>(targetHp * percent);
	return hp;
}
