#include "pch.h"
#include "Utilities.h"
#include "Config.h"
#include "Entity.h"
#include "EntityStates.h"
#include "TileMap.h"
#include "Directions.h"

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

sf::Vector2f Utilities::getEntityVisualPosition(const Entity& entity)
{
	auto pos = getEntityPos(entity);
	constexpr sf::Vector2f offset{ 40, 40 };
	return { pos + offset };
}

sf::Vector2i Utilities::getEntityCell(const Entity& entity)
{
	auto pos = getEntityVisualPosition(entity);
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

bool Utilities::isEntityWithinAttackRange(const Entity& attacker, const Entity& target)
{
	auto& statsComp = attacker.getComponent<CombatStatsComponent>();
	auto range = statsComp.cAttackRange;

	auto attackerCell = Utilities::getEntityCell(attacker);
	auto targetCell = Utilities::getEntityCell(target);

	auto dx = std::abs(attackerCell.x - targetCell.x);
	auto dy = std::abs(attackerCell.y - targetCell.y);
	return (dx <= range && dy == 0) || (dy <= range && dx == 0);
}

Direction Utilities::getDirectionToTarget(const Entity& entity, const Entity& target)
{
	auto fromCell = getEntityCell(entity);
	auto toCell = getEntityCell(target);
	
	int dx = toCell.x - fromCell.x;
	int dy = toCell.y - fromCell.y;

	if (dx == -1)
		return Direction::Left;
	else if (dx == 1)
		return Direction::Right;
	else if (dy == -1)
		return Direction::Up;
	else if (dy == 1)
		return Direction::Bottom;

	return Direction::Up;
}

void Utilities::setEntityDirection(const Entity& entity, Direction dir)
{
	entity.getComponent<DirectionComponent>().cCurrentDir = dir;
}

sf::Vector2f Utilities::calculateNewBarSize(const Entity& entity, const sf::Vector2f& originalSize)
{
	const auto& statsComp = entity.getComponent<CombatStatsComponent>();
	if (statsComp.cHealth <= 0)
		return { 0.f, originalSize.y };

	float fCurrHp = static_cast<float>(statsComp.cHealth);
	float fMaxHp = static_cast<float>(statsComp.cMaxHealth);

	float hpRatio = fCurrHp / fMaxHp;
	float xSize = hpRatio * originalSize.x;

	return sf::Vector2f{ xSize, originalSize.y };
}
