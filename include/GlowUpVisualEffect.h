#pragma once
#include "IVisualEffect.h"

class GlowUpVisualEffect :
    public IVisualEffect
{
public:
    GlowUpVisualEffect(const Entity& entity, int duration, const sf::Color& targetColor);
    // Inherited via IVisualEffect
    virtual void update(const sf::Time& deltaTime) override;
    virtual void render(sf::RenderWindow& window) override;
protected:
    virtual void updateEffect(const sf::Time& deltaTime) override;
    virtual void onEffectFinish() override;
private:
    float getProgress() const;
private:
    sf::Sprite& mSprite;
    const sf::Color mTargetColor;
    const sf::Color mStartingColor;
};

