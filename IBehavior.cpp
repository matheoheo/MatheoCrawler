#include "pch.h"
#include "IBehavior.h"
#include "Utilities.h"
#include "TileMap.h"
#include "Entity.h"
#include "Random.h"
#include "DelayTask.h"
#include "WaitUntilIdleTask.h"
#include "PatrolTask.h"

IBehavior::IBehavior(BehaviorContext& behaviorContext)
	:mBehaviorContext(behaviorContext)
{
}

void IBehavior::updateTasks(Entity& entity, const sf::Time& deltaTime)
{
	if (isTaskQueued())
	{
		updateFrontTask(entity, deltaTime);
		popCompletedTasks();
		return;
	}
	determineNextTask(entity);
}

void IBehavior::updateTimers(Entity& entity, const sf::Time& deltaTime)
{
	auto& comp = entity.getComponent<AITimersComponent>();
	comp.cTimeSinceLastLOSCheck += deltaTime.asMilliseconds();
}

void IBehavior::pushTask(std::unique_ptr<ITask> task)
{
	mTasks.emplace_back(std::move(task));
}

void IBehavior::updateFrontTask(Entity& entity, const sf::Time & deltaTime)
{
	mTasks.front()->update(entity, mBehaviorContext.eventManager, deltaTime);
}

void IBehavior::popCompletedTasks()
{
	while (isTaskQueued() && mTasks.front()->isComplete())
		mTasks.pop_front();
}

bool IBehavior::isTaskQueued() const
{
	return !mTasks.empty();
}

bool IBehavior::isCellReachable(const sf::Vector2i& targetCell) const
{
	constexpr std::array<sf::Vector2i, 4> offsets =
	{{
		{-1, 0}, {1, 0}, {0, -1}, {0, 1}
	}};
	for (const auto& offset : offsets)
	{
		int nx = targetCell.x + offset.x;
		int ny = targetCell.y + offset.y;

		if (mBehaviorContext.tileMap.isTileWalkable(nx, ny))
			return true;
	}
	return false;
}

bool IBehavior::isCellReachable(const sf::Vector2f& targetPos) const
{
	auto targetCell = Utilities::getCellIndex(targetPos);
	return isCellReachable(targetCell);
}

bool IBehavior::isEntityReachable(const Entity& entity) const
{
	return isCellReachable(Utilities::getEntityCell(entity));
}

void IBehavior::pushDelayTask(int interval)
{
	pushTask(std::make_unique<DelayTask>(interval));
}

int IBehavior::getRandomDelay(int minInterval) const
{
	int maxInterval = minInterval + static_cast<int>(minInterval * 0.3);
	return Random::get(minInterval, maxInterval);
}

void IBehavior::swapToPatrol()
{
	//pushDelayTask(getRandomDelay(150));
	pushTask(std::make_unique<WaitUntilIdleTask>());
	pushTask(std::make_unique<PatrolTask>());
	pushDelayTask(getRandomDelay(250));
}

bool IBehavior::canAttack(const Entity& entity, const Entity& target) const
{
	//We need to check if entity is close enough to attack
	//Then, if this is true, we cast Line Of Sight to determine if there is visibility.
	bool isInRange = Utilities::isEntityWithinAttackRange(entity, target);
	bool isInLOS = false;
	if (isInRange)
	{
		auto casterCell = Utilities::getEntityCell(entity);
		auto targetCell = Utilities::getEntityCell(target);
		isInLOS = mBehaviorContext.tileMap.isLineOfSightClear(casterCell, targetCell);
	}

	return isInRange && isInLOS;
}