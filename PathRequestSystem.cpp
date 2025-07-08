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

void PathRequestSystem::render(sf::RenderWindow& window)
{
	for (const auto& [id, vec] : paths)
	{
		for (const auto& r : vec)
			window.draw(r);
	}
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
	createPathVisual(entity);

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

void PathRequestSystem::createPathVisual(Entity& entity)
{
	auto& pathComp = entity.getComponent<PathComponent>();
	auto createRect = [](const sf::Vector2i& cell, const sf::Color& color)
	{
		sf::RectangleShape r;
		r.setSize({ 16.f, 16.f });
		r.setOrigin({ 8.f, 8.f });
		r.setFillColor(color);
		r.setPosition({ cell.x * 64.f + 32.f, cell.y * 64.f + 32.f });
		return r;
	};

	sf::Color color{
		static_cast<uint8_t>(Random::get(0, 255)),
		static_cast<uint8_t>(Random::get(0, 255)),
		static_cast<uint8_t>(Random::get(0, 255))
	};
	auto entId = entity.getEntityId();
	auto it = paths.find(entId);
	if (it == paths.end())
	{
		paths.emplace(entId, std::vector<sf::RectangleShape>());
	}
	else
	{
		it->second.clear();
	}

	auto& cells = pathComp.cPathCells;
	for (const auto& cell : cells)
	{
		paths.at(entId).emplace_back(createRect(cell, color));
	}

}
