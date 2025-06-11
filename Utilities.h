#pragma once
#include <SFML/System/Vector2.hpp>
class Entity;
class TileMap;
struct Tile;
enum class Direction;

namespace Utilities
{
	float getDistanceBetween(const sf::Vector2f& pointA, const sf::Vector2f& pointB);
	float getDistanceBetween(const sf::Vector2i& pointA, const sf::Vector2i& pointB);

	sf::Vector2i getCellIndex(const sf::Vector2f& pos);
	sf::Vector2f getEntityPos(const Entity& entity);
	sf::Vector2i getEntityCell(const Entity& entity);

	bool isEntityIdling(const Entity& entity);

	const sf::Color& getVisibleTileColor(const Tile& tile);
	const sf::Color& getExploredTileColor(const Tile& tile);

	bool isEntityWithinFOVRange(const Entity& observer, const Entity& target);
	bool isEntityWithinAttackRange(const Entity& attacker, const Entity& target);

	//returns direction, between entity to target, so that entity will stand 'face to face' with target
	Direction getDirectionToTarget(const Entity& entity, const Entity& target);
	void setEntityDirection(const Entity& entity, Direction dir);
};

