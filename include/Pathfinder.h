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

	PathfinderResult getPath(const sf::Vector2f& positionA, const sf::Vector2f& positionB, bool ignoreLastCell = true);
	PathfinderResult getPath(const sf::Vector2i& cellA, const sf::Vector2i& cellB, bool ignoreLastCell = true);
private:
	std::vector<PathNode*> getPathNodes(PathNode* start, PathNode* finish, bool ignoreLastCell = true);
	std::vector<PathNode*> getNeighbors(const PathNode* node, const PathNode* finish, bool ignoreLastCell = true);

	bool isIndexValid(const sf::Vector2i& index) const;
	bool isNodeWalkable(const sf::Vector2i& index) const;
	bool isNodeWalkable(const PathNode* node) const;
	bool isNodeWalkableRaw(const PathNode* node) const;
	int calculateGuessCost(const PathNode* from, const PathNode* to) const;
	void resetNode(PathNode& node);
	void resetLastlyUsedNodes();

	std::vector<PathNode*> reconstructPath(PathNode* finish);
private:
	const TileMap& mTileMap;
	std::vector<std::vector<PathNode>> mNodes;
	std::unordered_set<PathNode*> mLastlyUsedNodes;
	const std::vector<std::vector<Tile>>* mTiles;
};

