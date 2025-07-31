#include "pch.h"
#include "IAOESpell.h"
#include "Utilities.h"
#include "TileMap.h"
#include "Entity.h"

IAOESpell::IAOESpell(const Entity& caster, const TileMap& tileMap, const sf::Vector2f& castPos)
    :mCaster(caster),
    mTileMap(tileMap),
    mCastPos(castPos),
    mCastTime(1),
    mTimePassed(0),
    mIsComplete(false)
{
}

bool IAOESpell::isCompleted() const
{
    return mIsComplete;
}

void IAOESpell::addTimePassed(const sf::Time& deltaTime)
{
    mTimePassed += deltaTime.asMilliseconds();
}

bool IAOESpell::hasCastFinished() const
{
    return mTimePassed >= mCastTime;
}

void IAOESpell::complete()
{
    mIsComplete = true;
}

float IAOESpell::getProgressRatio() const
{
    return std::clamp(static_cast<float>(mTimePassed) / static_cast<float>(mCastTime), 0.0f, 1.0f);
}

std::vector<const Tile*> IAOESpell::getAffectedTiles(const std::vector<sf::Vector2i>& offsets, const sf::Vector2f& aroundPos) const
{
    std::vector<const Tile*> result;
    auto castCell = Utilities::getCellIndex(aroundPos);

    for (const auto& offset : offsets)
    {
        int nx = castCell.x + offset.x;
        int ny = castCell.y + offset.y;

        const Tile* tile = mTileMap.getTile(nx, ny);
        if (!tile || !tile->isWalkableRaw())
            continue;

        result.push_back(tile);
    }

    return result;
}

std::vector<Entity*> IAOESpell::getAffectedEntities(const std::vector<sf::Vector2i>& offsets, const sf::Vector2f& aroundPos) const
{
    std::vector<Entity*> entities;
    auto affectedTiles = getAffectedTiles(offsets, aroundPos);
    for (const Tile* tile : affectedTiles)
    {
        auto effectedEnts = mTileMap.getEntitiesOnTile(*tile);
        for (Entity* ent : effectedEnts)
            entities.push_back(ent);
    }

    return entities;
}

const sf::Vector2f& IAOESpell::getCasterPos() const
{
    return mCaster.getComponent<PositionComponent>().cLogicPosition;
}
