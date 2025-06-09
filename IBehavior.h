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
protected:
	BehaviorContext& mBehaviorContext;
	std::deque<std::unique_ptr<ITask>> mTasks;
};

