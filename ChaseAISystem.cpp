#include "pch.h"
#include "ChaseAISystem.h"
#include "Utilities.h"

ChaseAISystem::ChaseAISystem(SystemContext& systemContext, const TileMap& tileMap)
	:ISystem(systemContext),
	mTileMap(tileMap),
	mPathRecalculationCooldown(4000.f)
{
	registerToEvents();
}

void ChaseAISystem::update(const sf::Time& deltaTime)
{
	for (const auto& ent : mTrackedEntities)
	{
		auto& chaseComp = ent->getComponent<ChaseAIComponent>();
		auto& pathComp = ent->getComponent<PathComponent>();
		chaseComp.cTimeSinceLastRecalculation += deltaTime.asMilliseconds();

		auto nextStep = getPathStep(pathComp);
		if (!nextStep)
		{
			askForPathRecalculation(*ent);
			continue;
		}
		doStep(*ent, pathComp, nextStep.value());

		if (isPathRecalculationDue(chaseComp))
			askForPathRecalculation(*ent);
	}
	removeNotChasingEntities();
}

void ChaseAISystem::registerToEvents()
{
	registerToStartChasingEvent();
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
			mTrackedEntities.push_back(&data.entity);
		});
}

void ChaseAISystem::resetChaseComponent(ChaseAIComponent& chaseComp, Entity* target)
{
	chaseComp.cTarget = target;
	chaseComp.cTimeSinceLastRecalculation = 0.f;
}

void ChaseAISystem::resetPathComponent(PathComponent& pathComp)
{
	pathComp.cPathCells.clear();
}

std::optional<sf::Vector2i> ChaseAISystem::getPathStep(PathComponent& pathComp) const
{
	if (pathComp.cPathCells.empty())
		return std::nullopt;

	return pathComp.cPathCells.front();
}

bool ChaseAISystem::isStepWalkable(const Entity& entity, const sf::Vector2i& stepCell) const
{
	return mTileMap.isTileWalkable(stepCell.x, stepCell.y);
}

void ChaseAISystem::doStep(Entity& entity, PathComponent& pathComp, const sf::Vector2i& stepCell)
{
	if (!Utilities::isEntityIdling(entity))
	{
		return;

	}
	auto entPos = entity.getComponent<SpriteComponent>().cSprite.getPosition();
	auto entCell = Utilities::getCellIndex(entPos);

	auto nextStepDir = getStepDirection(entCell, stepCell);
	if (!nextStepDir)
		return;

	mSystemContext.eventManager.notify<MoveRequestedEvent>(MoveRequestedEvent(entity, nextStepDir.value()));
	pathComp.cPathCells.pop_front();
}

std::optional<Direction> ChaseAISystem::getStepDirection(const sf::Vector2i& fromCell, const sf::Vector2i& toCell) const
{
	int dx = toCell.x - fromCell.x;
	int dy = toCell.y - fromCell.y;

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

bool ChaseAISystem::isPathRecalculationDue(const ChaseAIComponent& chaseComp) const
{
	return chaseComp.cTimeSinceLastRecalculation >= mPathRecalculationCooldown;
}

void ChaseAISystem::askForPathRecalculation(Entity& entity)
{
	mSystemContext.eventManager.notify<RequestPathEvent>(RequestPathEvent(entity));
}

void ChaseAISystem::removeNotChasingEntities()
{
	std::erase_if(mTrackedEntities, [this](const Entity* ent)
		{
			return ent->getComponent<EntityAIStateComponent>().cState != EntityAIState::Chasing;
		});
}

