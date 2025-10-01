#pragma once
#include "SpellData.h"
class TileMap;
class EventManager;
class Entity;
struct Tile;

//Interface Area Of Effect Spell
class IAOESpell
{
public:
	IAOESpell(const Entity& caster, const TileMap& tileMap, const sf::Vector2f& castPos);

	virtual ~IAOESpell() = default;
	virtual void update(const sf::Time& deltaTime, EventManager& eventManager) = 0;
	virtual void render(sf::RenderWindow& window) = 0;

	bool isCompleted() const;
	void setCustomDamage(int damage);
protected:
	void addTimePassed(const sf::Time& deltaTime);
	bool hasCastFinished() const;
	void complete();
	float getProgressRatio() const;

	std::vector<const Tile*> getAffectedTiles(const std::vector<sf::Vector2i>& offsets, const sf::Vector2f& aroundPos) const;
	std::vector<Entity*> getAffectedEntities(const std::vector<sf::Vector2i>& offsets, const sf::Vector2f& aroundPos) const;
	const sf::Vector2f& getCasterPos() const;

	void hitEntities(std::vector<Entity*> entities, SpellIdentifier id, EventManager& eventManager,
		SpellEffect effect = SpellEffect::None);
protected:
	const Entity& mCaster;
	const TileMap& mTileMap;
	sf::Vector2f mCastPos;
	int mCastTime; //Time after spell's effect takes impact.
	int mTimePassed;
	bool mIsComplete;
	std::optional<int> mDamage;
};

