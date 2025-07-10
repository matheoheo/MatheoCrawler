#pragma once
#include "BehaviorContext.h"
#include "ITask.h"
enum class Direction;
enum class AnimationIdentifier;

class IBehavior
{
public:
	IBehavior(BehaviorContext& behaviorContext);
	virtual ~IBehavior() = default;
	virtual void update(Entity& entity, const sf::Time& deltaTime) = 0;
protected:
	virtual void determineNextTask(Entity& entity) = 0;
	virtual void swapToTargetting(Entity& entity) = 0;
	virtual void swapToAttacking(Entity& entity, Entity& target) = 0;
	virtual void fallbackOnNoDirection(Entity& self, Entity& target) = 0;

	void updateTasks(Entity& entity, const sf::Time& deltaTime);
	void updateTimers(Entity& entity, const sf::Time& deltaTime);

	void pushTask(std::unique_ptr<ITask> task);
	void updateFrontTask(Entity& entity, const sf::Time& deltaTime);
	void popCompletedTasks();
	bool isTaskQueued() const;

	bool isCellReachable(const sf::Vector2i& targetCell) const;
	bool isCellReachable(const sf::Vector2f& targetPos) const;

	//returns true if entity has walkable tiles around him(4 directions)
	//returns false if all tiles around entity(4 directions) are not walkable
	bool isEntityReachable(const Entity& entity) const;

	//most delay tasks are used specifically to not swap states immediately
	//this gives more realism, and 'natural' feeling of AI.
	void pushDelayTask(int interval);

	//returns random delay between [minInterval, minInterval * 1.3]
	//used so different entities dont swap their states at the same time.
	int getRandomDelay(int minInterval) const;

	//swaps entity state to patrol state
	void swapToPatrol();

	bool isTargetInFOV(const Entity& entity, const Entity& target) const;
	virtual bool canReachEntity(const Entity& entity, const Entity& target) const;
	bool canTargetEntity(const Entity& entity, const Entity& target) const;
	bool canAttack(const Entity& entity, const Entity& target) const;
	bool canCastLOS(const Entity& entity) const;
	void resetLOSTimer(const Entity& entity) const;
	bool setDirectionTowardTarget(Entity& self, Entity& target);
	std::optional<Direction> getDirectionToTarget(const Entity& self, const Entity& target) const;

	void handleLogicIfPatrolling(Entity& entity, Entity& player);
	AnimationIdentifier getRandomAttack(const Entity& entity);
protected:
	BehaviorContext& mBehaviorContext;
	std::deque<std::unique_ptr<ITask>> mTasks;
};

