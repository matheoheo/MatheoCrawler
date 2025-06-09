#include "pch.h"
#include "IMapGenerator.h"

const IMapGenerator::SpawnPoints& IMapGenerator::getSpawnPoints() const
{
    return mSpawnPoints;
}
