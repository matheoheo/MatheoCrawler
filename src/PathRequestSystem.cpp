#include "pch.h"
#include "PathRequestSystem.h"
#include "Utilities.h"
#include "Random.h"

PathRequestSystem::PathRequestSystem(SystemContext& systemContext, Pathfinder& pathfinder)
	:ISystem(systemContext),
	mPathfinder(pathfinder)
{
	registerToEvents();
}

void PathRequestSystem::update(const sf::Time& deltaTime)
{
	int maxRecalculations = 2; //we recalculate for maximally 2 entities per frame
	for (const auto& entity : mTrackedEntities)
	{
		auto& pathComp = entity->getComponent<PathComponent>();
		pathComp.cTimeSinceLastRecalculation += deltaTime.asMilliseconds();
		if (maxRecalculations == 0)
			break;

		bool idling = Utilities::isEntityIdling(*entity);
		bool timePassed = hasRecalculationIntervalPassed(*entity);
		//we recalculate only if entity is in an idle state, and the path wasn't recalculated already.
		if (!idling || !timePassed)
			continue;

		updatePathToTarget(*entity);
		mFinishedRequestsIds.emplace_back(entity->getEntityId());
		--maxRecalculations;
	}
	removeFinishedEntities();
}

void PathRequestSystem::registerToEvents()
{
	registerToPathRequestEvent();
}

void PathRequestSystem::registerToPathRequestEvent()
{
	mSystemContext.eventManager.registerEvent<RequestPathEvent>([this](const RequestPathEvent& data)
		{
			auto& pathComp = data.entity.getComponent<PathComponent>();
			pathComp.cPathCells.clear();
			pathComp.cTargetCell = data.destinationCell;
			if (isEntityAlreadyTracked(data.entity))
				return;

			mTrackedEntities.push_back(&data.entity);

		});
}

void PathRequestSystem::updatePathToTarget(Entity& entity)
{
	auto& pathComp = entity.getComponent<PathComponent>();
	auto entityCell = Utilities::getEntityCell(entity);
	auto targetCell = pathComp.cTargetCell;
	auto path = mPathfinder.getPath(entityCell, targetCell);
	pathComp.cPathCells.assign(std::begin(path), std::end(path));

	mSystemContext.eventManager.notify<AddToPathFollowSystemEvent>(AddToPathFollowSystemEvent(entity));

	if (!pathComp.cPathCells.empty())
	{
		auto frontCell = pathComp.cPathCells.front();
		if (frontCell == entityCell)
			pathComp.cPathCells.pop_front();
	}
}

void PathRequestSystem::removeFinishedEntities()
{
	std::erase_if(mTrackedEntities, [this](const Entity* ent)
		{
			return std::ranges::find(mFinishedRequestsIds, ent->getEntityId()) != std::ranges::end(mFinishedRequestsIds);
		});
	mFinishedRequestsIds.clear();
}

bool PathRequestSystem::hasRecalculationIntervalPassed(const Entity& entity) const
{
	constexpr int minRecalculationInterval = 500; //milliseconds
	auto& chaseComp = entity.getComponent<PathComponent>();
	return chaseComp.cTimeSinceLastRecalculation > minRecalculationInterval;
}

