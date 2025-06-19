#pragma once
#include "ISystem.h"
#include "Random.h"

/*
* This class might need a bit of refactoring in future if i decide to add more effects.
*/
class EffectSystem :
    public ISystem
{
public:
    EffectSystem(SystemContext& systemContext);
    // Inherited via ISystem
    virtual void update(const sf::Time& deltaTime) override;
    virtual void render(sf::RenderWindow& window);
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
    struct RegenerationEffect {
        struct RegenVisual {
            sf::RectangleShape vertical;
            sf::RectangleShape horizontal;
            sf::Vector2f dirVector;
            int lifetime = 0;
            RegenVisual(const sf::Vector2f& pos, const sf::Color& color) {
                constexpr sf::Vector2f size{ 8, 3 };
                horizontal.setSize(size);
                vertical.setSize({ size.y, size.x });
                horizontal.setFillColor(color);
                vertical.setFillColor(color);
                horizontal.setOrigin({ size.x / 2, size.y / 2 });
                vertical.setPosition(pos);
                horizontal.setPosition(pos + vertical.getGeometricCenter());

                int randomVec = Random::get(-30, 30);
                dirVector = { static_cast<float>(randomVec) / 100.f, -1.0f };
            }
            void update(const sf::Time& deltaTime) {
                constexpr float mSpeed = 50;
                vertical.move(dirVector * mSpeed * deltaTime.asSeconds());
                horizontal.move(dirVector * mSpeed * deltaTime.asSeconds());
                ++lifetime;

                //we also reduce alpha transparency
                updateColor(vertical);
                updateColor(horizontal);
            }
            void render(sf::RenderWindow& window) {
                window.draw(vertical);
                window.draw(horizontal);
            }

            bool lifetimePassed() const {
                const int kMaxlifetime = 100; //100 frames
                return lifetime >= kMaxlifetime;
            }
            
            void updateColor(sf::RectangleShape& shape) {
                auto color = shape.getFillColor();
                if(color.a > 1)
                    color.a -= 2;
                shape.setFillColor(color);
            }
        };
        const Entity* entity;
        sf::Color effectColor;
        int timer = 0;
        int effectDuration = 1; //in ms
        int spawnTimer = 0; //in frame count
        std::vector<RegenVisual> visuals;

        RegenerationEffect(const Entity& entity, const sf::Color& color, int duration)
            :entity(&entity),
            effectColor(color),
            effectDuration(duration)
        {
            visuals.reserve(15);
        }
    };
    void registerToEvents();
    void registerToStartGlowEffect();
    void regToEntityDiedEvent();
    void registerToSpawnRegenerationEffect();

    void removeEffects(Entity::EntityID id);
    void removeRegenerationEffect(Entity::EntityID id);
    void removeGlowEffect(Entity::EntityID id);
    void removeFinishedEffects();

    void updateGlowEffects(const sf::Time& deltaTime);
    bool hasEffectFinished(const GlowEffect& effect) const;
    void onEffectFinish(GlowEffect& effect);
    float getEffectProgress(const GlowEffect& effect) const;
    void updateEffect(GlowEffect& effect, float progress);

    //regeneration effect data
    bool hasEffectFinished(const RegenerationEffect& effect) const;
    bool updateRegenerationEffects(const sf::Time& deltaTime);
    bool isRegenVisualSpawnDue(const RegenerationEffect& effect) const;
    void spawnRegenerationVisual(RegenerationEffect& effect);
    void updateRegenVisuals(RegenerationEffect& effect, const sf::Time& deltaTime);
    void renderRegenerationEffects(sf::RenderWindow& window);

    sf::Vector2f getSpawnPos(const RegenerationEffect& effect) const;
private:
    std::vector<GlowEffect> mGlowEffects;
    std::vector<Entity::EntityID> mFinishedGlows;

    std::vector<RegenerationEffect> mRegenerationEffects;
    std::vector<Entity::EntityID> mFinishedRegenerations;

    const int mRegenEffectInterval; //frame count that must pass between spawning more visuals.
};

