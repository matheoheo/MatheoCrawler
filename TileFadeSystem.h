#pragma once
#include "ISystem.h"
#include "Tile.h"

class TileFadeSystem :
    public ISystem
{
public:
    TileFadeSystem(SystemContext& systemContext);
    virtual void update(const sf::Time& deltaTime) override;
private:
    void registerToEvents();
    void registerToTileFadeRequestEvent();
    void removeFinishedTiles();
    void setTileColor(Tile& tile, const sf::Color& color);
    void markTileAsFinished(Tile& tile);
    
    void makeVisibleFade(Tile& tile);
    void makeExploredFade(Tile& tile);
    bool shouldKeepFading(Tile& tile) const;
    void fadeTile(Tile& tile);

    sf::Color lerpColor(const sf::Color& startColor, const sf::Color& endColor, float t) const;
    bool isColorCloseEnough(const sf::Color& currColor, const sf::Color& targetColor, int threshold = 2) const;

private:
    std::unordered_set<Tile*> mFadingTiles;
    std::vector<Tile*> mFinishedFadingTiles;
};

