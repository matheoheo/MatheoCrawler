#pragma once
#include "SpellData.h"
class TileMap;
class EventManager;
struct Tile;

//Interface Area Of Effect Spell
class IAOESpell
{
public:
	IAOESpell(const TileMap& tileMap, const sf::Vector2f& castPos);

	virtual ~IAOESpell() = default;
	virtual void update(const sf::Time& deltaTime, EventManager& eventManager) = 0;
	virtual void render(sf::RenderWindow& window) = 0;

	bool isCompleted() const;
protected:
	void addTimePassed(const sf::Time& deltaTime);
	bool hasCastFinished() const;
	void complete();
	float getProgressRatio() const;

	std::vector<const Tile*> getAffectedTiles(const std::vector<sf::Vector2i>& offsets) const;
protected:
	const TileMap& mTileMap;
	const sf::Vector2f mCastPos;
	int mCastTime; //Time after spell's effect takes impact.
	int mTimePassed;
	bool mIsComplete;
};

