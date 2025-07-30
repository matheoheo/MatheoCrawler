#include "pch.h"
#include "CollisionSystem.h"
#include "Config.h"
#include "TileMap.h"
#include "Utilities.h"

CollisionSystem::CollisionSystem(SystemContext& systemContext, const TileMap& tileMap)
	:ISystem(systemContext),
	mTileMap(tileMap)
{
	registerToEvents();
}

void CollisionSystem::update(const sf::Time& deltaTime)
{
	for (const auto& entity : mTrackedEntities)
	{
		if (!Utilities::isEntityIdling(*entity) || hasEntityMovementdBlocked(*entity))
			continue;

		auto nextPos = getNextTilePosition(*entity);
		if (mTileMap.isTileWalkable(nextPos))
			mSystemContext.eventManager.notify<MoveAllowedEvent>(MoveAllowedEvent(*entity, nextPos));
	}
	mTrackedEntities.clear();
}

void CollisionSystem::registerToEvents()
{
	mSystemContext.eventManager.registerEvent<MoveRequestedEvent>([this](const MoveRequestedEvent& data)
		{
			auto& dirComp = data.entity.getComponent<DirectionComponent>();
			dirComp.cNextDir = data.dir;
			mTrackedEntities.push_back(&data.entity);
		});
}

bool CollisionSystem::hasEntityMovementdBlocked(const Entity& entity) const
{
	return entity.getComponent<MovementComponent>().cMovementBlocked;
}

sf::Vector2f CollisionSystem::getNextTilePosition(const Entity& entity) const
{
	auto currPos = entity.getComponent<SpriteComponent>().cSprite.getPosition();
	auto nextDir = entity.getComponent<DirectionComponent>().cNextDir;
	sf::Vector2f mod;

	switch (nextDir)
	{
	case Direction::Up:
		mod.y = -Config::getCellSize().y;
		break;
	case Direction::Bottom:
		mod.y = Config::getCellSize().y;
		break;
	case Direction::Left:
		mod.x = -Config::getCellSize().x;
		break;
	case Direction::Right:
		mod.x = Config::getCellSize().x;
		break;
	default:
		break;
	}
	return currPos + mod;
}
