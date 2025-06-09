#include "pch.h"
#include "IBehavior.h"

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
