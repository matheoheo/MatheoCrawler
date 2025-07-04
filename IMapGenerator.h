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
protected:
	SpawnPoints mSpawnPoints;
private:
};

