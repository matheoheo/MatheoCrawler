#pragma once
#include "ISystem.h"
class EffectSystem :
    public ISystem
{
public:
    EffectSystem(SystemContext& systemContext);
    // Inherited via ISystem
    virtual void update(const sf::Time& deltaTime) override;
private:
    struct GlowEffect {
        const Entity* entity; //using pointers over references, because erase_if doesnt work with non MoveAssignable types.
        Entity::EntityID entId;
        sf::Color onStartColor;
        sf::Color targetColor;
        sf::Sprite* sprite;
        int timer = 0;
        int effectDuration = 1;

        GlowEffect(const Entity& entity, const sf::Color& targetColor, int duration)
            :entity(&entity),
            sprite(&entity.getComponent<SpriteComponent>().cSprite),
            entId(entity.getEntityId()),
            targetColor(targetColor),
            effectDuration(duration)
        {
            onStartColor = sprite->getColor();
        }
    };
    void registerToEvents();
    void registerToStartGlowEffect();
    //need to add removing effects on entity death
    void removeEffect(Entity::EntityID id);
    void removeFinishedEffects();

    bool hasEffectFinished(const GlowEffect& effect) const;
    void onEffectFinish(GlowEffect& effect);
    float getEffectProgress(const GlowEffect& effect) const;
    void updateEffect(GlowEffect& effect, float progress);
private:
    std::vector<GlowEffect> mGlowEffects;
    std::vector<Entity::EntityID> mFinishedGlows;
};

