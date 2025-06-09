#include "pch.h"
#include "BehaviorAIUpdateSystem.h"
#include "IBehavior.h"
BehaviorAIUpdateSystem::BehaviorAIUpdateSystem(SystemContext& systemContext, const TileMap& tileMap)
	:ISystem(systemContext),
	mTileMap(tileMap)
{
	registerToEvents();
}

void BehaviorAIUpdateSystem::update(const sf::Time& deltaTime)
{
	auto& entities = mTileMap.getVisibleEntities();

	for (const auto& entity : entities)
	{
		if (entity->hasComponent<PlayerComponent>())
			continue;

		auto& behaviorComp = entity->getComponent<BehaviorComponent>();
		if (behaviorComp.cBehavior)
		{
			behaviorComp.cBehavior->update(*entity, deltaTime);
		}
	}
}

void BehaviorAIUpdateSystem::registerToEvents()
{
	registerToMonsterAppearEvent();
	registerToMonsterDisappearEvent();
}

void BehaviorAIUpdateSystem::registerToMonsterAppearEvent()
{
	mSystemContext.eventManager.registerEvent<MonsterAppearedEvent>([this](const MonsterAppearedEvent& data)
		{
			if (!data.entity.hasComponent<EntityAIStateComponent>())
				return;

			//changeEntityAIState(data.entity, EntityAIState::Patrolling);
			//mSystemContext.eventManager.notify<StartPatrollingEvent>(StartPatrollingEvent(data.entity));
		});
}

void BehaviorAIUpdateSystem::registerToMonsterDisappearEvent()
{
	mSystemContext.eventManager.registerEvent<MonsterDisappearedEvent>([this](const MonsterDisappearedEvent& data)
		{
			if (!data.entity.hasComponent<EntityAIStateComponent>())
				return;

			changeEntityAIState(data.entity, EntityAIState::None);
		});
}

void BehaviorAIUpdateSystem::changeEntityAIState(Entity& entity, EntityAIState state)
{
	entity.getComponent<EntityAIStateComponent>().cState = state;
}
