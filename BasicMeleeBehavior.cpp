#include "pch.h"
#include "BasicMeleeBehavior.h"
#include "Utilities.h"
#include "PatrolTask.h"
#include "DelayTask.h"
#include "ChaseTask.h"
#include "EntityManager.h"
#include "EventManager.h"
#include "TileMap.h"
#include "WaitUntilIdleTask.h"
#include "AttackTask.h"
#include "Random.h"
#include "AnimationIdentifiers.h"
#include "TurnToTargetTask.h"

BasicMeleeBehavior::BasicMeleeBehavior(BehaviorContext& behaviorContext)
	:IBehavior(behaviorContext)
{

}

void BasicMeleeBehavior::update(Entity& entity, const sf::Time& deltaTime)
{
	updateTimers(entity, deltaTime);
	updateTasks(entity, deltaTime);
}

void BasicMeleeBehavior::determineNextTask(Entity& entity)
{
	auto& aiStateComp = entity.getComponent<EntityAIStateComponent>();
	auto& player = mBehaviorContext.entityManager.getPlayer();
	auto state = aiStateComp.cState;

	if (state != EntityAIState::Attacking && canAttack(entity, player))
	{
		swapToAttacking(entity, player);
		return;
	}
	if (state == EntityAIState::None)
	{
		swapToPatrol();
	}
	else if (state == EntityAIState::Patrolling)
	{
		handleLogicIfPatrolling(entity, player);
	}
	else if (state == EntityAIState::Chasing)
	{
		handleLogicIfChasing(entity, player);
	}
	else if (state == EntityAIState::Attacking)
	{
		handleLogicIfAttacking(entity, player);
	}
	pushDelayTask(getRandomDelay(90));
}

void BasicMeleeBehavior::swapToTargetting(Entity& entity)
{
	pushTask(std::make_unique<WaitUntilIdleTask>());
	pushTask(std::make_unique<ChaseTask>(entity));
	pushDelayTask(getRandomDelay(250));
}

void BasicMeleeBehavior::swapToAttacking(Entity& entity, Entity& target)
{
	auto attackType = getRandomAttack(entity);
	pushTask(std::make_unique<WaitUntilIdleTask>());
	pushTask(std::make_unique<TurnToTargetTask>(target, this));
	pushTask(std::make_unique<AttackTask>(attackType));
	pushDelayTask(getRandomDelay(250));
}

void BasicMeleeBehavior::fallbackOnNoDirection(Entity& self, Entity& target)
{

}

void BasicMeleeBehavior::handleLogicIfChasing(Entity& entity, Entity& player)
{
	if (canAttack(entity, player))
	{
		swapToAttacking(entity, player);
		return;
	}

	auto& chaseComp = entity.getComponent<ChaseAIComponent>();
	if (!chaseComp.cTarget)
	{
		//if there is no target to chase, then just go back to patrol?
		swapToPatrol();
		return;
	}

	bool isReachable = isEntityReachable(*chaseComp.cTarget);
	bool pathExists = mBehaviorContext.tileMap.doesPathExist(entity, *chaseComp.cTarget);
	if (isReachable && pathExists)
		chaseComp.cUnreachableRetryCount = 0;

	if (!isReachable  || !pathExists)
	{
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

	bool inFOVRange = isTargetInFOV(*chaseComp.cTarget, player);
	if (!inFOVRange)
	{
		//if target moved outside fov range, we just go back to patrolling
		swapToPatrol();
	}
}

void BasicMeleeBehavior::handleLogicIfAttacking(Entity& entity, Entity& player)
{
	if (canAttack(entity, player))
	{
		if (entity.getComponent<EntityStateComponent>().cCurrentState == EntityState::Idle)
		{
			swapToAttacking(entity, player);
		}
	}
	else
	{
		swapToTargetting(player);
	}
}

