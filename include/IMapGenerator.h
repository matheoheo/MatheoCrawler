#pragma once
#include <vector>
#include "TileTypes.h"
#include <SFML/System/Vector2.hpp>

class IMapGenerator
{
public:
	using GeneratedMap = std::vector<std::vector<TileType>>;
	using SpawnPoints = std::vector<sf::Vector2i>;
	virtual ~IMapGenerator() = default;
	[[nodiscard]] virtual GeneratedMap generate(const sf::Vector2i& size, int maxDepth,
		const sf::Vector2i& minRoomSize, const sf::Vector2i& maxRoomSize) = 0;

	const SpawnPoints& getSpawnPoints() const;
	const sf::Vector2i& getSpawnCell() const;
	const sf::Vector2i& getNextLevelCell() const;
protected:
	void carveTunnel(GeneratedMap& map, const sf::Vector2i& pointA, const sf::Vector2i& pointB);
	virtual void setSpawnCell(const GeneratedMap& map) = 0;
	virtual void setNextLevelCell(const GeneratedMap& map) = 0;

	bool isAFloor(TileType type) const;
	bool isAWall(TileType type) const;
	void printMap(const GeneratedMap& map);
protected:
	SpawnPoints mSpawnPoints;
	sf::Vector2i mNextLevelCell;
	sf::Vector2i mSpawnCell;
private:
};

