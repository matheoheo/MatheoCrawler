#pragma once
#include "IMapGenerator.h"

/*
	Generating caves usign Cellular Automata algorithm.
*/

class CaveGenerator
	:public IMapGenerator
{
public:
	// Inherited via IMapGenerator
	virtual GeneratedMap generate(const sf::Vector2i& size, int maxDepth, const sf::Vector2i& minRoomSize, const sf::Vector2i& maxRoomSize) override;
private:
	void initalize(GeneratedMap& map, const sf::Vector2i& size);
	void setMapBordersAsWall(GeneratedMap& map);
	void populateWallsRandomly(GeneratedMap& map);
	void performCARound(GeneratedMap& map);
	void generateCA(GeneratedMap& map);
	std::vector<std::vector<sf::Vector2i>> findCaves(GeneratedMap& map);
	std::vector<sf::Vector2i> floodFill(const GeneratedMap& map, std::vector<std::vector<bool>>& visited, int x, int y);
	void connectCaves(GeneratedMap& generatedMap);
	int getWallCount(const GeneratedMap& map, int x, int y) const;
	
};