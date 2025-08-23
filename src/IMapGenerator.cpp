#include "pch.h"
#include "IMapGenerator.h"
#include "Random.h"

const IMapGenerator::SpawnPoints& IMapGenerator::getSpawnPoints() const
{
    return mSpawnPoints;
}

const sf::Vector2i& IMapGenerator::getNextLevelCell() const
{
    return mNextLevelCell;
}

void IMapGenerator::carveTunnel(GeneratedMap& map, const sf::Vector2i& pointA, const sf::Vector2i& pointB)
{
    constexpr TileType floor = TileType::Floor;
    bool orderOfConnection = Random::get(0, 1);
    if (orderOfConnection)
    {
        for (int x = std::min(pointA.x, pointB.x); x <= std::max(pointA.x, pointB.x); ++x)
            map[pointA.y][x] = floor;

        for (int y = std::min(pointA.y, pointB.y); y <= std::max(pointA.y, pointB.y); ++y)
            map[y][pointB.x] = floor;
    }
    else
    {
        for (int y = std::min(pointA.y, pointB.y); y <= std::max(pointA.y, pointB.y); ++y)
            map[y][pointA.x] = floor;

        for (int x = std::min(pointA.x, pointB.x); x <= std::max(pointA.x, pointB.x); ++x)
            map[pointB.y][x] = floor;
    }
}
