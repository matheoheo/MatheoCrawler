#pragma once
#include "IAOESpell.h"
class BladeDanceSpell :
    public IAOESpell
{
public:
    BladeDanceSpell(const Entity& caster, const TileMap& tileMap, const sf::Vector2f& castPos);

    // Inherited via IAOESpell
    virtual void update(const sf::Time& deltaTime, EventManager& eventManager) override;
    virtual void render(sf::RenderWindow& window) override;
private:
    struct BladeDanceConfig
    {
        struct Timing{
            static constexpr int spellDuration = 3000;
            static constexpr int damageTickInterval = 500;
        };
        struct Visual{
            static constexpr int bladeCount = 4;
            static constexpr sf::Vector2f bladeSize{ 18.f, 6.f };
            static constexpr sf::Color friendlyColor{ 0, 150, 100, 200 };
            static constexpr sf::Color enemyColor{ 255, 80, 80, 200 };
            static constexpr float bladeSpeed = 128.f;
            static constexpr float bladeRadius = 48.f;
            static constexpr float rotationSpeedRad = std::numbers::pi_v<float>;
        };
    };
    using Timing = BladeDanceConfig::Timing;
    using Visual = BladeDanceConfig::Visual;

    bool isTimeToMakeDamage() const;
    void makeDamage(EventManager& eventManager);
private:
    struct BladeRect
    {
        sf::RectangleShape shape;
        sf::Angle angle;
    };

    BladeRect createBladeRect(sf::Angle angle, bool isFriendly) const;
    void createBlades();
    void updateBlade(BladeRect& blade, const sf::Time& deltaTime);
    void updateAllBlades(const sf::Time& deltaTime);
private:    
    int mTicksCount;
    std::vector<BladeRect> mBlades;
};

