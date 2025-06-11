#pragma once
#include "BehaviorContext.h"
#include "ITask.h"

class IBehavior
{
public:
	IBehavior(BehaviorContext& behaviorContext);
	virtual ~IBehavior() = default;
	virtual void update(Entity& entity, const sf::Time& deltaTime) = 0;
protected:
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
protected:
	BehaviorContext& mBehaviorContext;
	std::deque<std::unique_ptr<ITask>> mTasks;
};

