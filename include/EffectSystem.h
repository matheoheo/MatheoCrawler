#pragma once
#include "ISystem.h"
#include "IVisualEffect.h"

class EffectSystem :
    public ISystem
{
public:
    EffectSystem(SystemContext& systemContext);
    // Inherited via ISystem
    virtual void update(const sf::Time& deltaTime) override;
    virtual void render(sf::RenderWindow& window);
private:
    void registerToEvents();
    void registerToStartGlowEffect();
    void regToEntityDiedEvent();
    void registerToSpawnRegenerationEffect();

    void removeEffects(Entity::EntityID id);
private:
    std::vector<std::unique_ptr<IVisualEffect>> mEffects;

};

