#include "pch.h"
#include "Utilities.h"
#include "Config.h"
#include "Entity.h"
#include "EntityStates.h"
#include "TileMap.h"

float Utilities::getDistanceBetween(const sf::Vector2f& pointA, const sf::Vector2f& pointB)
{
	return std::hypotf(pointA.x - pointB.x, pointA.y - pointB.y);
}

float Utilities::getDistanceBetween(const sf::Vector2i& pointA, const sf::Vector2i& pointB)
{
	const sf::Vector2f fA = { static_cast<float>(pointA.x), static_cast<float>(pointA.y) };
	const sf::Vector2f fB = { static_cast<float>(pointB.x), static_cast<float>(pointB.y) };

	return getDistanceBetween(fA, fB);
}

sf::Vector2i Utilities::getCellIndex(const sf::Vector2f& pos)
{
	int cellX = static_cast<int>(pos.x) / static_cast<int>(Config::getCellSize().x);
	int cellY = static_cast<int>(pos.y) / static_cast<int>(Config::getCellSize().y);
	return { cellX, cellY };
}

sf::Vector2f Utilities::getEntityPos(const Entity& entity)
{
	return entity.getComponent<SpriteComponent>().cSprite.getPosition();
}

sf::Vector2i Utilities::getEntityCell(const Entity& entity)
{
	auto pos = entity.getComponent<SpriteComponent>().cSprite.getPosition();
	return getCellIndex(pos);
}

bool Utilities::isEntityIdling(const Entity& entity)
{
	return entity.getComponent<EntityStateComponent>().cCurrentState == EntityState::Idle;
}

const sf::Color& Utilities::getVisibleTileColor(const Tile& tile)
{
	if (tile.tileType == TileType::Wall)
		return Config::wallTileColor;

	return Config::floorTileColor;
}

const sf::Color& Utilities::getExploredTileColor(const Tile& tile)
{
	if (tile.tileType == TileType::Wall)
		return Config::dimmedWallTileColor;

	return Config::dimmedFloorTileColor;
}

bool Utilities::isEntityWithinFOVRange(const Entity& observer, const Entity& target)
{
	auto observerCell = Utilities::getEntityCell(observer);
	auto targetCell = Utilities::getEntityCell(target);

	auto dx = std::abs(observerCell.x - targetCell.x);
	auto dy = std::abs(observerCell.y - targetCell.y);
	auto fovRange = observer.getComponent<FieldOfViewComponent>().cRangeOfView;

	return dx <= fovRange && dy <= fovRange;
}
