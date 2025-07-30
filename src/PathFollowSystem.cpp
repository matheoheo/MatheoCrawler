#include "pch.h"
#include "PathFollowSystem.h"
#include "Utilities.h"
#include "TileMap.h"

PathFollowSystem::PathFollowSystem(SystemContext& systemContext, TileMap& tileMap)
	:ISystem(systemContext),
	mTileMap(tileMap)
{
	registerToEvents();
}

void PathFollowSystem::update(const sf::Time& deltaTime)
{
	for (const auto& ent : mTrackedEntities)
	{
		auto& pathComp = ent->getComponent<PathComponent>();
		if (isPathEmpty(pathComp))
		{
			markAsFinished(ent);
			continue;
		}
		doStep(*ent, pathComp);
	}
	removeFinishedEntites();
}

void PathFollowSystem::registerToEvents()
{
	registerToAddToPathFollowSystemEvent();
}

void PathFollowSystem::registerToAddToPathFollowSystemEvent()
{
	mSystemContext.eventManager.registerEvent<AddToPathFollowSystemEvent>([this](const AddToPathFollowSystemEvent& data)
		{
			if (isEntityAlreadyTracked(data.entity))
				return;

			mTrackedEntities.push_back(&data.entity);
		});
}

std::optional<sf::Vector2i> PathFollowSystem::getPathStep(const PathComponent& pathComp) const
{
	if (pathComp.cPathCells.empty())
		return {};

	return pathComp.cPathCells.front();
}

std::optional<Direction> PathFollowSystem::getStepDirection(const Entity& ent, const sf::Vector2i& stepCell) const
{
	auto entCell = Utilities::getEntityCell(ent);
	int dx = stepCell.x - entCell.x;
	int dy = stepCell.y - entCell.y;

	if (std::abs(dx) > 1 || std::abs(dy) > 1)
		return std::nullopt;

	if (dx == -1)
		return Direction::Left;
	else if (dx == 1)
		return Direction::Right;
	else if (dy == -1)
		return Direction::Up;
	else if (dy == 1)
		return Direction::Bottom;

	return std::nullopt;
}

void PathFollowSystem::doStep(Entity& entity, PathComponent& pathComp)
{
	if (!Utilities::isEntityIdling(entity))
		return;

	auto nextCell = pathComp.cPathCells.front();
	if (!mTileMap.isTileWalkable(nextCell.x, nextCell.y))
	{
		//should happen very rarely, but if happens then just recalculate path again to target cell.
		abortPath(entity);
		return;
	}
	auto nextDir = getStepDirection(entity, nextCell);
	if (!nextDir)
	{
		abortPath(entity);
		return;
	}
	mSystemContext.eventManager.notify<MoveRequestedEvent>(MoveRequestedEvent(entity, nextDir.value()));
	pathComp.cPathCells.pop_front();
}

bool PathFollowSystem::isPathEmpty(const PathComponent& pathComp) const
{
	return pathComp.cPathCells.empty();
}

void PathFollowSystem::abortPath(Entity& ent)
{
	auto& pathComp = ent.getComponent<PathComponent>();
	pathComp.cPathCells.clear();
	pathComp.cPathAborted = true;
	mSystemContext.eventManager.notify<PathAbortedEvent>(PathAbortedEvent(ent));
	markAsFinished(&ent);
}

void PathFollowSystem::markAsFinished(Entity* ent)
{
	mFinishedEntities.insert(ent);
}

void PathFollowSystem::removeFinishedEntites()
{
	std::erase_if(mTrackedEntities, [this](Entity* entity)
		{
			return mFinishedEntities.contains(entity);
		});
	mFinishedEntities.clear();
}
