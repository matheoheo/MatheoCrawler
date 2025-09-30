#pragma once
#include "ISystem.h"
#include "BeamSpell.h"

class TileMap;

class BeamSystem
	:public ISystem
{
public:
	BeamSystem(SystemContext& systemContext, const TileMap& tileMap);

	virtual void update(const sf::Time& deltaTime) override;
	virtual void render(sf::RenderWindow& window) override;
private:
	void registerToEvents();
	void registerToCastBeamEvent();
	void removeCompletedBeams();
private:
	using AffectedTiles = std::vector<const Tile*>;
	using SpellPair = std::pair<int, AffectedTiles>;

	//Returns real length of beam (reduced by SOLID tiles), and tiles that the spell will take place on.
	SpellPair getSpellPair(const BeamData& beamData) const;
private:
	const TileMap& mTileMap;
	std::vector<std::unique_ptr<BeamSpell>> mBeamSpells;

};