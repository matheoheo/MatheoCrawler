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
		auto start = std::chrono::high_resolution_clock::now();

		auto& behaviorComp = entity->getComponent<BehaviorComponent>();
		if (behaviorComp.cBehavior)
			behaviorComp.cBehavior->update(*entity, deltaTime);
	
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = end - start;
		//if(elapsed.count() > 1.0)
			//std::cout << "Update  time per entity: " << elapsed.count() << " ms\n";

	}
}

void BehaviorAIUpdateSystem::registerToEvents()
{
	registerToMonsterAppearEvent();
	registerToMonsterDisappearEvent();
}

void BehaviorAIUpdateSystem::registerToMonsterAppearEvent()
{
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
