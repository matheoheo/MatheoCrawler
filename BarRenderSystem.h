#pragma once
#include "ISystem.h"
class BarRenderSystem :
    public ISystem
{
public:
    BarRenderSystem(SystemContext& systemContext);

    // Inherited via ISystem
    virtual void update(const sf::Time& deltaTime) override;
    virtual void render(sf::RenderWindow& window) override;
private:
    void registerToEvents();
    void registerToUpdateHealthBarEvent();

    sf::Vector2f calculateNewBarSize(const Entity& entity, const sf::Vector2f& originalSize) const;
    void updateBarPosition(const Entity& entity);
private:
    void removeFinishedEntities();
};

