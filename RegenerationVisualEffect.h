#pragma once
#include "IVisualEffect.h"
class RegenerationVisualEffect :
    public IVisualEffect
{
public:
    RegenerationVisualEffect(const Entity& entity, int duration, const sf::Color& efectColor);
    // Inherited via IVisualEffect
    virtual void update(const sf::Time& deltaTime) override;
    virtual void render(sf::RenderWindow& window) override;
protected:
    virtual void updateEffect(const sf::Time& deltaTime) override;
private:
    //This effect looks like '+'.
    //To achieve this, we just use 2 rectangles
    struct RegenVisual {
        sf::RectangleShape vertical;
        sf::RectangleShape horizontal;
        sf::Vector2f dirVector;
        int lifeTime = 0; //in frames amount
    };

    RegenVisual spawnVisualEffect(const sf::Vector2f& pos) const;
    void updateRegenVisual(RegenVisual& effect, const sf::Time& deltaTime);
    void updateEffectTransparency(sf::RectangleShape& rect);
    bool hasRegenVisualLifetimePassed(const RegenVisual& effect);
    void removeFinishedVisuals();
    bool isEffectSpawnDue() const;

    sf::Vector2f getSpawnPos() const;
private:
    const sf::Color mEffectColor;
    const int mEffectLifetimeLimit; //in frames.
    std::vector<RegenVisual> mVisuals;
    int mLastSpawn;
};

