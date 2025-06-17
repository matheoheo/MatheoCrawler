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
	//sometimes entities might change their position because of animations offsets
	//because of this - other functions return wrong cell on grid
	//this function returns original position modified by {40, 40} offset.
	sf::Vector2f getEntityVisualPosition(const Entity& entity);
	sf::Vector2i getEntityCell(const Entity& entity);

	bool isEntityIdling(const Entity& entity);

	const sf::Color& getVisibleTileColor(const Tile& tile);
	const sf::Color& getExploredTileColor(const Tile& tile);

	bool isEntityWithinFOVRange(const Entity& observer, const Entity& target);
	bool isEntityWithinAttackRange(const Entity& attacker, const Entity& target);

	//returns direction, between entity to target, so that entity will stand 'face to face' with target
	Direction getDirectionToTarget(const Entity& entity, const Entity& target);
	void setEntityDirection(const Entity& entity, Direction dir);

	//calculates health bar size based on health ratio.
	sf::Vector2f calculateNewBarSize(const Entity& entity, const sf::Vector2f& originalSize);
	sf::Vector2f calculateNewBarSize(const sf::Vector2f& originalSize, int minV, int maxV);
	void changeHpBarSize(const Entity& entity, const sf::Vector2f& newSize);
	void changeBarSize(sf::RectangleShape& rect, const sf::Vector2f& newSize);
	void scaleSprite(sf::Sprite& sprite, const sf::Vector2f& newSize);
};

