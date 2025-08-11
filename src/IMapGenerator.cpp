#include "pch.h"
#include "IMapGenerator.h"

const IMapGenerator::SpawnPoints& IMapGenerator::getSpawnPoints() const
{
    return mSpawnPoints;
}

const sf::Vector2i& IMapGenerator::getNextLevelCell() const
{
    return mNextLevelCell;
}
