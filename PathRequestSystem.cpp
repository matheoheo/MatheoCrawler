#include "pch.h"
#include "PathRequestSystem.h"
#include "Utilities.h"

PathRequestSystem::PathRequestSystem(SystemContext& systemContext, Pathfinder& pathfinder)
	:ISystem(systemContext),
	mPathfinder(pathfinder),
	mMinRecalculationInterval(500.f)
{
	registerToEvents();
}

void PathRequestSystem::update(const sf::Time& deltaTime)
{
	int maxRecalculations = 2; //we recalculate for maximally 2 entities per frame
	for (const auto& entity : mTrackedEntities)
	{
		if (maxRecalculations == 0)
			break;

		//we recalculate only if entity is in an idle state, and the path wasn't recalculated already.
		if (!Utilities::isEntityIdling(*entity) || !hasRecalculationIntervalPassed(*entity))
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
			if(isEntityAlreadyTracked(data.entity))
				return;
			//clear previous path
			data.entity.getComponent<PathComponent>().cPathCells.clear();
			mTrackedEntities.push_back(&data.entity);
		});
}

void PathRequestSystem::updatePathToTarget(const Entity& entity)
{
	auto& chaseComp = entity.getComponent<ChaseAIComponent>();
	auto& pathComp = entity.getComponent<PathComponent>();

	if (!chaseComp.cTarget)
		return;
	
	auto entityPos = Utilities::getEntityPos(entity);
	auto targetPos = Utilities::getEntityPos(*chaseComp.cTarget);
	auto path = mPathfinder.getPath(entityPos, targetPos);
	chaseComp.cTargetReachableByPath = !path.empty();

	//std::cout << "Found path of size: " << path.size() << std::endl;

	pathComp.cPathCells.assign(std::begin(path), std::end(path));
	chaseComp.cTimeSinceLastRecalculation = 0.f;
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
	auto& chaseComp = entity.getComponent<ChaseAIComponent>();
	return chaseComp.cTimeSinceLastRecalculation > mMinRecalculationInterval;
}
