#pragma once
#include "ISystem.h"

//We render only entities that are in field of view of player.
class EntityRenderSystem :
    public ISystem
{
public:
    EntityRenderSystem(SystemContext& systemContext);
    // Inherited via ISystem
    virtual void update(const sf::Time& deltaTime) override;
    virtual void render(sf::RenderWindow& window) override;
private:
    void registerToEvents();
    void registerToUpdateEntityRenderTilesEvent();
    void determineRenderedEntities(const std::vector<Tile*>& tiles);
private:
    std::vector<Entity*> mRenderedEntities;
    int mFramesSinceLastRecalculation; //to prevent frequent recalculations
};

