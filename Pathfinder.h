#pragma once
#include "TileMap.h"

class Pathfinder
{
public:
	using PathfinderResult = std::vector<sf::Vector2i>; //result is a vector of cell indexes
	struct PathNode
	{
		PathNode* parent;
		sf::Vector2i cellIndex;
		int localCost;
		int guessCost;
		bool visited;
		int getFullCost() const { return localCost + guessCost; }

		PathNode(const sf::Vector2i& index) 
			:parent(nullptr),
			localCost(1500),
			guessCost(0),
			cellIndex(index),
			visited(false)
		{}
	};
public:
	Pathfinder(const TileMap& tileMap);
	void initalize();
	void setSolidTypes(const std::vector<TileType>& solidTypes);

	PathfinderResult getPath(const sf::Vector2f& positionA, const sf::Vector2f& positionB);
	PathfinderResult getPath(const sf::Vector2i& cellA, const sf::Vector2i& cellB);
private:
	std::vector<PathNode*> getPathNodes(PathNode* start, PathNode* finish);
	std::vector<PathNode*> getNeighbors(const PathNode* node, const PathNode* finish);

	bool isIndexValid(const sf::Vector2i& index) const;
	bool isNodeWalkable(const sf::Vector2i& index) const;
	int calculateGuessCost(const PathNode* from, const PathNode* to) const;
	void resetNode(PathNode& node);
	void resetLastlyUsedNodes();

	std::vector<PathNode*> reconstructPath(PathNode* finish);
private:
	const TileMap& mTileMap;
	std::vector<TileType> mSolidTypes;
	std::vector<std::vector<PathNode>> mNodes;
	std::unordered_set<PathNode*> mLastlyUsedNodes;
	const std::vector<std::vector<Tile>>* mTiles;
};

