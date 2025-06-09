#pragma once
#include "ISystem.h"
#include "TileMap.h"

class BehaviorAIUpdateSystem :
    public ISystem
{
public:
    BehaviorAIUpdateSystem(SystemContext& systemContext, const TileMap& tileMap);
    virtual void update(const sf::Time& deltaTime) override;
private:
    void registerToEvents();
    void registerToMonsterAppearEvent();
    void registerToMonsterDisappearEvent();

    void changeEntityAIState(Entity& entity, EntityAIState state);
private:
    const TileMap& mTileMap;
};

