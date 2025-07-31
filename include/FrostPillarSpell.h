#pragma once
#include "IAOESpell.h"

class FrostPillarSpell :
    public IAOESpell
{
public:
    FrostPillarSpell(const Entity& caster, const TileMap& tileMap, const sf::Vector2f& castPos);
    // Inherited via IAOESpell
    virtual void update(const sf::Time& deltaTime, EventManager& eventManager) override;
    virtual void render(sf::RenderWindow& window) override;
private:
    struct FrozenTileEffect
    {
        sf::RectangleShape frozenShape;
        sf::VertexArray crackLine; //cracks on tile, so the effect doesn't look so cheap
    };
    struct FrostColors
    {
        static constexpr sf::Color fade { 60, 100, 150, 200 };
        static constexpr sf::Color aura { 120, 200, 215, 130  };
        static constexpr sf::Color cracks{ 30, 50, 80, 220 };
    };

    void initEffects();
    void renderEffects(sf::RenderWindow& window);
    FrozenTileEffect createEffect(const sf::Vector2f& pos);
    void updateEffect(FrozenTileEffect& effect, const sf::Time& deltaTime);
private:
    void onCastFinish(EventManager& eventManager);
    sf::Color getColorMod(const sf::Color& color, std::uint8_t alpha) const;
    sf::Vector2f getRandomPosWithinCell(const sf::Vector2f& cellPos) const;
private:
    std::vector<FrozenTileEffect> mFrozenEffects;
    bool mUpdateEffect;
};

