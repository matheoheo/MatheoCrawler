#pragma once
#include "TileTypes.h"
#include <SFML/Graphics.hpp>
#include "Entity.h"
#include "Config.h"

struct Tile
{
	sf::RectangleShape tileVisual;
	TileType tileType;
	std::vector<Entity*> occupyingEntities;
	Entity* reservedEntity; //is tile reserved
	bool explored;
	bool visible;

	Tile() : tileType(TileType::None), reservedEntity(nullptr), explored(false), visible(false) {}

	Tile(const sf::Vector2f& position, TileType tileType, const sf::Vector2f& size = { 63.f, 63.f })
		:tileType(tileType),
		reservedEntity(nullptr),
		explored(false),
		visible(false)
	{
		if (tileType == TileType::Floor)
		{
			tileVisual.setOutlineThickness(0.5f);
		}
		tileVisual.setPosition(position);
		tileVisual.setSize(size);
		switch (tileType)
		{
		case TileType::Floor: tileVisual.setFillColor(Config::floorTileColor); break;
		case TileType::Wall: tileVisual.setFillColor(Config::wallTileColor); break;
		default: tileVisual.setSize({ 0.f, 0.f }); break;
		}
	}

	inline bool isWalkable() const
	{
		return tileType == TileType::Floor && occupyingEntities.empty() && !reservedEntity;
	}

	inline void determineTileColor()
	{
		if (tileType == TileType::None)
			return;

		if (visible)
		{
			tileVisual.setFillColor(tileType == TileType::Floor ? Config::floorTileColor : Config::wallTileColor);
		}
		else if (explored)
		{
			tileVisual.setFillColor(tileType == TileType::Floor ? Config::dimmedFloorTileColor : Config::dimmedWallTileColor);
		}
	}
};