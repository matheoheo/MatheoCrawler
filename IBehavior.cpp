#include "pch.h"
#include "IBehavior.h"
#include "Utilities.h"
#include "TileMap.h"
#include "Entity.h"
#include "Random.h"
#include "DelayTask.h"

IBehavior::IBehavior(BehaviorContext& behaviorContext)
	:mBehaviorContext(behaviorContext)
{
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
	int maxInterval = minInterval + minInterval * 0.3;
	return Random::get(minInterval, maxInterval);
}
