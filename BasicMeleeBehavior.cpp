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

bool BasicMeleeBehavior::canChaseEntity(const Entity& entity, const Entity& target) const
{
	if(!Utilities::isEntityWithinFOVRange(entity, target))
		return false;

	auto entityCell = Utilities::getEntityCell(entity);
	auto playerCell = Utilities::getEntityCell(target);
	
	resetLOSTimer(entity);
	return mBehaviorContext.tileMap.isLineOfSightClear(entityCell, playerCell);
}

bool BasicMeleeBehavior::canCastLOS(const Entity& entity) const
{
	constexpr float LOSCheckCooldown = 200.f;
	return entity.getComponent<AITimersComponent>().cTimeSinceLastLOSCheck >= LOSCheckCooldown;
}

void BasicMeleeBehavior::swapToChase(Entity& target)
{
	//pushDelayTask(getRandomDelay(150));
	pushTask(std::make_unique<WaitUntilIdleTask>());
	pushTask(std::make_unique<ChaseTask>(target));
	pushDelayTask(getRandomDelay(250));

}

void BasicMeleeBehavior::swapToAttacking(Entity& entity, Entity& target)
{
	auto dir = Utilities::getDirectionToTarget(entity, target);
	Utilities::setEntityDirection(entity, dir);
	auto attackType = getRandomAttack(entity);

	//pushDelayTask(getRandomDelay(150));
	pushTask(std::make_unique<WaitUntilIdleTask>());
	pushTask(std::make_unique<AttackTask>(attackType));
	pushDelayTask(getRandomDelay(250));
}

void BasicMeleeBehavior::handleLogicIfPatrolling(Entity& entity, Entity& player)
{
	//if last light of sight was casted less than 1 second ago, we do not want to repeat it right now
	//let it time out a bit
	if (canCastLOS(entity))
	{
		if (!isEntityReachable(player))
		{
			//if entity is patrolling, but the target (player) is not reachable (has only non walkable tiles around him)
			//we add delay to prevent every frame rethinking.
			pushDelayTask(getRandomDelay(150));
			return;
		}
		if (canChaseEntity(entity, player) && mBehaviorContext.tileMap.doesPathExist(entity, player))
		{
			swapToChase(player);
		}
	}
	else
		pushDelayTask(getRandomDelay(200));
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

	bool inFOVRange = Utilities::isEntityWithinFOVRange(*chaseComp.cTarget, player);
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
		swapToChase(player);
	}
}

void BasicMeleeBehavior::resetLOSTimer(const Entity& entity) const
{
	entity.getComponent<AITimersComponent>().cTimeSinceLastLOSCheck = 0.f;

}

AnimationIdentifier BasicMeleeBehavior::getRandomAttack(const Entity& entity)
{
	auto& avAttVec = entity.getComponent<AvailableAttacksComponent>().mAttacks;
	int size = avAttVec.size();
	if (size == 0)
		return AnimationIdentifier::Attack1;
	else if (size == 1)
		return avAttVec[0];

	int randomAttack = Random::get(0, size - 1);
	return avAttVec[randomAttack];
}
