#include "pch.h"
#include "BasicRangedBehavior.h"
#include "EntityManager.h"
#include "EventManager.h"
#include "MessageTypes.h"
#include "WaitUntilIdleTask.h"
#include "PositionToAttackTask.h"
#include "AttackTask.h"
#include "Utilities.h"

BasicRangedBehavior::BasicRangedBehavior(BehaviorContext& behaviorContext)
	:IBehavior(behaviorContext)
{
}

void BasicRangedBehavior::update(Entity& entity, const sf::Time& deltaTime)
{
	updateTimers(entity, deltaTime);
	updateTasks(entity, deltaTime);
}

void BasicRangedBehavior::determineNextTask(Entity& entity)
{
	auto& entStateComp = entity.getComponent<EntityAIStateComponent>();
	auto state = entStateComp.cState;
	auto& target = mBehaviorContext.entityManager.getPlayer();
	
	auto& positioningComp = entity.getComponent<PositioningComponent>();
	if (positioningComp.cLastRepositionTryFailed)
	{
		//if we failed to find correct position to reposition - we must add some delay to prevent spamming.
		pushDelayTask(500);
		positioningComp.cLastRepositionTryFailed = false;
		return;
	}

	if (isTargetTooClose(entity, target))
	{
		swapToTargetting(target);
	}
	else if (isTargetTooFar(entity, target))
	{
		if (state != EntityAIState::Patrolling)
			swapToPatrol();
	}

	if (state == EntityAIState::None)
	{
		if (canAttack(entity, target))
		{
			swapToAttacking(entity, target);
			return;
		}
		swapToPatrol();
	}
	else if (state == EntityAIState::Patrolling)
	{
		handleLogicIfPatrolling(entity, target);
	}
	else if (state == EntityAIState::RepositionToAttack)
	{
		if (canAttack(entity, target))
		{
			swapToAttacking(entity, target);
		}
		else
		{
			bool cellValid = isTargetCellValid(entity, target);
			auto& pathComp = entity.getComponent<PathComponent>();
			bool isIdling = Utilities::isEntityIdling(entity);

			if (!cellValid || (isIdling && pathComp.cPathCells.empty()))
			{
				//retry?
				swapToTargetting(target);
				return;
			}

			if (pathComp.cPathAborted)
			{
				pathComp.cPathAborted = false;
				swapToPatrol();
				return;
			}
			
		}
	}
	else if (state == EntityAIState::Attacking)
	{
		if (!canAttack(entity, target))
		{
			swapToTargetting(target);
		}
		else
		{
			if (entity.getComponent<EntityStateComponent>().cCurrentState == EntityState::Idle)
				swapToAttacking(entity, target);
		}
	}
}

void BasicRangedBehavior::swapToTargetting(Entity& target)
{
	pushTask(std::make_unique<WaitUntilIdleTask>());
	pushTask(std::make_unique<PositionToAttackTask>(target));
	pushDelayTask(250);
}

void BasicRangedBehavior::swapToAttacking(Entity& entity, Entity& target)
{
	if (!setDirectionTowardTarget(entity, target))
		return;

	//entity.getComponent<EntityAIStateComponent>().cState = EntityAIState::Attacking;
	std::string msg = std::format("Entity of id {} has swapped to attacking.", entity.getEntityId());
	mBehaviorContext.eventManager.notify<LogMessageEvent>(LogMessageEvent(MessageType::Custom, {}, msg));
	pushTask(std::make_unique<WaitUntilIdleTask>());
	pushTask(std::make_unique<AttackTask>(AnimationIdentifier::Attack1));
	pushDelayTask(getRandomDelay(250));

}

void BasicRangedBehavior::fallbackOnNoDirection(Entity& self, Entity& target)
{
	swapToTargetting(target);
}

bool BasicRangedBehavior::canReachEntity(const Entity& entity, const Entity& target) const
{
	std::cout << "RANGED BEHAVIOR\n";
	return true;
}

bool BasicRangedBehavior::isTargetTooClose(Entity& self, Entity& target) const
{
	auto myCell = Utilities::getEntityCell(self);
	auto targetCell = Utilities::getEntityCell(target);

	int minRange = self.getComponent<PositioningComponent>().cMinRange;
	int dx = std::abs(myCell.x - targetCell.x);
	int dy = std::abs(myCell.y - targetCell.y);

	return dx < minRange && dy < minRange;
}

bool BasicRangedBehavior::isTargetTooFar(Entity& self, Entity& target) const
{
	auto myCell = Utilities::getEntityCell(self);
	auto targetCell = Utilities::getEntityCell(target);

	int maxRange = self.getComponent<PositioningComponent>().cMaxRange;
	int dx = std::abs(myCell.x - targetCell.x);
	int dy = std::abs(myCell.y - targetCell.y);

	return dx > maxRange || dy > maxRange;
}

bool BasicRangedBehavior::isTargetCellValid(Entity& self, Entity& target) const
{
	//When repositioning, we are saving targetCelll (targetCell = cell where 'self' is suppoused to reposition to)
	//this function returns wether this saved target cell is still valid as place to attack.

	const auto& posComp = self.getComponent<PositioningComponent>();
	const auto range = self.getComponent<CombatStatsComponent>().cAttackRange;
	const auto savedTargetCell = posComp.cTargetCell;
	const auto currTargetCell = Utilities::getEntityCell(target);

	int dx = std::abs(savedTargetCell.x - currTargetCell.x);
	int dy = std::abs(savedTargetCell.y - currTargetCell.y);

	if (dx != 0 && dy != 0)
		return false;

	if (dx == 0)
		return dy <= range;

	return dx <= range;
}

