#pragma once
#include "IState.h"
#include "TileMap.h"
#include "EntityManager.h"
#include "DungeonGenerator.h"
#include "SystemContext.h"
#include "SystemManager.h"
#include "AnimationHolder.h"
#include "Pathfinder.h"
#include "BehaviorContext.h"

class GameState :
    public IState
{
public:
    GameState(GameContext& gameContext);

    // Inherited via IState
    virtual void onEnter() override;
    virtual void processEvents(sf::Event event) override;
    virtual void update(const sf::Time& deltaTime) override;
    virtual void render() override;
private:
    void createMap();
    void renderMap();
    void loadTextures();
    void createPlayer();
    void createSystems();
    void loadAnimations();
    void initalizePathfinder();
    void createSkeletonAxe(const sf::Vector2i& cellIndex);
    void spawnEntities();
private:
    sf::View mGameView;
    TileMap mTileMap;
    DungeonGenerator mGenerator;
    EntityManager mEntityManager;
    SystemContext mSystemContext;
    SystemManager mSystemManager;
    AnimationHolder mAnimationHolder;
    Pathfinder mPathfinder;
    BehaviorContext mBehaviorContext;
};

