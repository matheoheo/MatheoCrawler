#pragma once
#include "BeamData.h"
class TileMap;
struct Tile;
class EventManager;

class BeamSpell
{
public:
	BeamSpell(const BeamData& beamData, const TileMap& tileMap, const std::vector<const Tile*>& affectedTiles);
	void update(const sf::Time& deltaTime, EventManager& eventManager);
	void render(sf::RenderWindow& window);

	bool hasCompleted() const;
private:
	struct BeamSpellConfig
	{
		static constexpr float Thickness = 8.f;
	};

	float getBeamLength() const;
	sf::Vector2f getBeamPos() const;
	sf::Vector2f getBeamDimension() const;
	sf::Vector2f getBeamOrigin(const sf::Vector2f& size) const;
	sf::Vector2f getInnerStartingSize() const;

	void updateInnerBeam(float length);
	bool isVertical() const;
	void createBeam();
private:
	void addTimePassed(const sf::Time& deltaTime);
	float getProgress() const;
	bool hasTimePassed() const;
	void animateBeam();
	void pulseColor();
	void onBeamFinish(EventManager& eventManager);
private:
	BeamData mBeamData;
	const TileMap& mTileMap;
	const std::vector<const Tile*> mAffectedTiles;
	sf::RectangleShape mOuter;
	sf::RectangleShape mInner;
	int mTimeSinceStart; //in milliseconds
	bool mIsComplete;
};