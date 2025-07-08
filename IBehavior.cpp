#include "pch.h"
#include "IBehavior.h"
#include "Utilities.h"
#include "TileMap.h"
#include "Entity.h"
#include "Random.h"
#include "DelayTask.h"
#include "WaitUntilIdleTask.h"
#include "PatrolTask.h"
#include "Directions.h"
#include "AnimationIdentifiers.h"

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
	pushTask(std::make_unique<WaitUntilIdleTask>());
	pushTask(std::make_unique<PatrolTask>());
	pushDelayTask(getRandomDelay(250));
}

bool IBehavior::isTargetInFOV(const Entity& entity, const Entity& target) const
{
	return Utilities::isEntityWithinFOVRange(entity, target);
}

bool IBehavior::canReachEntity(const Entity& entity, const Entity& target) const
{
	std::cout << "IBEHAVIOR REACH\n";
	return mBehaviorContext.tileMap.doesPathExist(entity, target);
}

bool IBehavior::canTargetEntity(const Entity& entity, const Entity& target) const
{
	if (!isTargetInFOV(entity, target))
		return false;

	auto entityCell = Utilities::getEntityCell(entity);
	auto playerCell = Utilities::getEntityCell(target);

	resetLOSTimer(entity);
	return mBehaviorContext.tileMap.isLineOfSightClear(entityCell, playerCell);
}

bool IBehavior::canAttack(const Entity& entity, const Entity& target) const
{
	//We need to check if entity is close enough to attack
	//Then, if this is true, we cast Line Of Sight to determine if there is visibility.
	bool isInRange = Utilities::isEntityWithinAttackRange(entity, target);
	bool LineOfSightClear = false;
	if (isInRange)
	{
		auto casterCell = Utilities::getEntityCell(entity);
		auto targetCell = Utilities::getEntityCell(target);
		LineOfSightClear = mBehaviorContext.tileMap.isLineOfSightClear(casterCell, targetCell);
	}

	return isInRange && LineOfSightClear;
}

bool IBehavior::canCastLOS(const Entity& entity) const
{
	constexpr float LOSCheckCooldown = 200.f;
	return entity.getComponent<AITimersComponent>().cTimeSinceLastLOSCheck >= LOSCheckCooldown;
}

void IBehavior::resetLOSTimer(const Entity& entity) const
{
	entity.getComponent<AITimersComponent>().cTimeSinceLastLOSCheck = 0.f;
}

bool IBehavior::setDirectionTowardTarget(Entity& self, Entity& target)
{
	auto dir = getDirectionToTarget(self, target);
	if (!dir)
	{
		fallbackOnNoDirection(self, target);
		return false;
	}
	Utilities::setEntityDirection(self, dir.value());
	return true;
}

std::optional<Direction> IBehavior::getDirectionToTarget(const Entity& self, const Entity& target) const
{
	auto fromCell = Utilities::getEntityCell(self);
	auto toCell   = Utilities::getEntityCell(target);

	int dx = toCell.x - fromCell.x;
	int dy = toCell.y - fromCell.y;

	if (dx != 0 && dy != 0)
		return {};

	if (dx != 0)
		return dx > 0 ? Direction::Right : Direction::Left;
	else if (dy != 0)
		return dy > 0 ? Direction::Bottom : Direction::Up;

	return {};
}

void IBehavior::handleLogicIfPatrolling(Entity& entity, Entity& player)
{
	//if last light of sight was casted less than half second ago, we do not want to repeat it right now
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
		if (canTargetEntity(entity, player) && canReachEntity(entity, player))
		{
			swapToTargetting(player);
		}
	}
	else
		pushDelayTask(getRandomDelay(200));
}

AnimationIdentifier IBehavior::getRandomAttack(const Entity& entity)
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
