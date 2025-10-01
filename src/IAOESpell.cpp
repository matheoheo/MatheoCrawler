#include "pch.h"
#include "IAOESpell.h"
#include "Utilities.h"
#include "TileMap.h"
#include "Entity.h"
#include "SpellHolder.h"
#include "Random.h"

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

void IAOESpell::setCustomDamage(int damage)
{
    mDamage = damage;
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

void IAOESpell::hitEntities(std::vector<Entity*> entities, SpellIdentifier id, EventManager& eventManager, SpellEffect effect)
{
    const auto& spell = SpellHolder::getInstance().get(id);
    bool isPlayerCaster = mCaster.hasComponent<PlayerComponent>();
    for (Entity* ent : entities)
    {
        bool isPlayerHit = ent->hasComponent<PlayerComponent>();
        //if spell was casted by player and player would be hit - we skip,
        //as well if spell wasn't casted by player and it's not player that got hit
        //we do not allow friendly fire.
        if ((isPlayerCaster && isPlayerHit) || (!isPlayerCaster && !isPlayerHit))
            continue;

        int dmg = Random::get(spell.aoe->minDmg, spell.aoe->maxDmg);
        //if the damage was overriden then we use provided damage.
        if (mDamage)
            dmg = *mDamage;
        eventManager.notify<HitByAOESpellEvent>(HitByAOESpellEvent(*ent, dmg, isPlayerCaster));
        if (effect != SpellEffect::None)
            eventManager.notify<AddSpellEffectEvent>(AddSpellEffectEvent(*ent, effect));
    }
}
