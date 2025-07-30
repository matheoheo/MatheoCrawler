#include "pch.h"
#include "ChaseAISystem.h"
#include "Utilities.h"

ChaseAISystem::ChaseAISystem(SystemContext& systemContext, const TileMap& tileMap)
	:ISystem(systemContext),
	mTileMap(tileMap)
{
	registerToEvents();
}

void ChaseAISystem::update(const sf::Time& deltaTime)
{
	removeNotChasingEntities();

	for (const auto& ent : mTrackedEntities)
	{
		auto& chaseComp = ent->getComponent<ChaseAIComponent>();
		auto& pathComp = ent->getComponent<PathComponent>();
		pathComp.cTimeSinceLastRecalculation += deltaTime.asMilliseconds();
		/*
		auto nextStep = getPathStep(pathComp);
		if (!nextStep)
		{
			askForPathRecalculation(*ent);
			continue;
		}
		//doStep(*ent, pathComp, nextStep.value());

		if (isPathRecalculationDue(pathComp))
			askForPathRecalculation(*ent);*/
	}
}

void ChaseAISystem::registerToEvents()
{
	registerToStartChasingEvent();
	registerToPlayerMovedEvent();
	registerToPathAbortedEvent();
}

void ChaseAISystem::registerToStartChasingEvent()
{
	mSystemContext.eventManager.registerEvent<StartChasingEvent>([this](const StartChasingEvent& data)
		{
			auto& stateComp = data.entity.getComponent<EntityAIStateComponent>();
			if (stateComp.cState == EntityAIState::Chasing)
				return;

			stateComp.cState = EntityAIState::Chasing;
			auto& chaseComp = data.entity.getComponent<ChaseAIComponent>();
			auto& pathComp = data.entity.getComponent<PathComponent>();
			resetChaseComponent(chaseComp, &data.target);
			resetPathComponent(pathComp);
			askForPathRecalculation(data.entity);
			mTrackedEntities.push_back(&data.entity);
		});
}

void ChaseAISystem::registerToPlayerMovedEvent()
{
	mSystemContext.eventManager.registerEvent<PlayerMoveFinishedEvent>([this](const PlayerMoveFinishedEvent& data)
		{
			for (const auto& entity : mTrackedEntities)
				askForPathRecalculation(*entity);
		});
}

void ChaseAISystem::registerToPathAbortedEvent()
{
	mSystemContext.eventManager.registerEvent<PathAbortedEvent>([this](const PathAbortedEvent& data)
		{
			if (isEntityAlreadyTracked(data.entity))
			{
				askForPathRecalculation(data.entity);
			}
		});
}

void ChaseAISystem::resetChaseComponent(ChaseAIComponent& chaseComp, Entity* target)
{
	chaseComp.cTarget = target;
}

void ChaseAISystem::resetPathComponent(PathComponent& pathComp)
{
	pathComp.cPathCells.clear();
	pathComp.cTimeSinceLastRecalculation = 0;
}

bool ChaseAISystem::isPathRecalculationDue(const PathComponent& chaseComp) const
{
	constexpr int pathRecalcCooldown = 4000;
	return chaseComp.cTimeSinceLastRecalculation >= pathRecalcCooldown;
}

void ChaseAISystem::askForPathRecalculation(Entity& entity)
{
	auto& chaseComp = entity.getComponent<ChaseAIComponent>();
	if (!chaseComp.cTarget)
		return;
	auto targetCell = Utilities::getEntityCell(*chaseComp.cTarget);
	mSystemContext.eventManager.notify<RequestPathEvent>(RequestPathEvent(entity, targetCell));
}

void ChaseAISystem::removeNotChasingEntities()
{
	std::erase_if(mTrackedEntities, [this](const Entity* ent)
		{
			return ent->getComponent<EntityAIStateComponent>().cState != EntityAIState::Chasing;
		});
}

