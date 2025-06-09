#include "pch.h"
#include "BasicMeleeBehavior.h"
#include "Utilities.h"
#include "PatrolTask.h"
#include "DelayTask.h"
#include "ChaseTask.h"
#include "EntityManager.h"
#include "EventManager.h"
#include "TileMap.h"

BasicMeleeBehavior::BasicMeleeBehavior(BehaviorContext& behaviorContext)
	:IBehavior(behaviorContext)
{

}

void BasicMeleeBehavior::update(Entity& entity, const sf::Time& deltaTime)
{
	auto& comp = entity.getComponent<AITimersComponent>();
	comp.cTimeSinceLastLOSCheck += deltaTime.asMilliseconds();

	if (isTaskQueued())
	{
		updateFrontTask(entity, deltaTime);
		popCompletedTasks();
		return;
	}


	determineNextTask(entity);
}

void BasicMeleeBehavior::determineNextTask(Entity& entity)
{
	auto& aiStateComp = entity.getComponent<EntityAIStateComponent>();
	auto state = aiStateComp.cState;
	if (state == EntityAIState::None)
	{
		std::cout << "Added patrol task!\n";
		pushTask(std::make_unique<PatrolTask>());
		pushTask(std::make_unique<DelayTask>(1000));
	}
	else if (state == EntityAIState::Patrolling)
	{

		auto& player = mBehaviorContext.entityManager.getPlayer();
		if (canCastLOS(entity) && canChaseEntity(entity, player))
		{
			std::cout << "Added chase task!\n";
			pushTask(std::make_unique<ChaseTask>(player));
			pushTask(std::make_unique<DelayTask>(1000));
		}
	}
	else if (state == EntityAIState::Chasing)
	{

	}
}

bool BasicMeleeBehavior::canChaseEntity(const Entity& entity, const Entity& target) const
{
	if(!Utilities::isEntityWithinFOVRange(entity, target))
		return false;

	auto entityCell = Utilities::getEntityCell(entity);
	auto playerCell = Utilities::getEntityCell(target);
	//
	entity.getComponent<AITimersComponent>().cTimeSinceLastLOSCheck = 0.f;
	return mBehaviorContext.tileMap.isLineOfSightClear(entityCell, playerCell);
}

bool BasicMeleeBehavior::canCastLOS(const Entity& entity) const
{
	return entity.getComponent<AITimersComponent>().cTimeSinceLastLOSCheck >= 1000.f;
}
