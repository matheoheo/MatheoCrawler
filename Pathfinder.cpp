#include "pch.h"
#include "Pathfinder.h"
#include "Utilities.h"

Pathfinder::Pathfinder(const TileMap& tileMap)
	:mTileMap(tileMap),
	mTiles(nullptr)
{
}

void Pathfinder::initalize()
{
	mTiles = &mTileMap.getTiles();
	const auto mapHeight = mTiles->size();
	const auto mapWidth = (*mTiles)[0].size();

	mNodes.resize(mapHeight, std::vector<PathNode>(mapWidth, PathNode({ 0, 0 })));


	for (size_t y = 0; y < mapHeight; ++y)
	{
		for (size_t x = 0; x < mapWidth; ++x)
		{
			mNodes[y][x] = PathNode(sf::Vector2i(static_cast<int>(x), static_cast<int>(y)));
		}
	}
}

void Pathfinder::setSolidTypes(const std::vector<TileType>& solidTypes)
{
	mSolidTypes = solidTypes;
}

Pathfinder::PathfinderResult Pathfinder::getPath(const sf::Vector2f& positionA, const sf::Vector2f& positionB, bool ignoreLastCell)
{
	auto indexA = Utilities::getCellIndex(positionA);
	auto indexB = Utilities::getCellIndex(positionB);

	return getPath(indexA, indexB, ignoreLastCell);
}

Pathfinder::PathfinderResult Pathfinder::getPath(const sf::Vector2i& cellA, const sf::Vector2i& cellB, bool ignoreLastCell)
{
	PathfinderResult result;

	auto getProperNode = [this](const sf::Vector2i& index) -> Pathfinder::PathNode*
	{
		if (isIndexValid(index))
			return &mNodes[index.y][index.x];

		return nullptr;
	};

	PathNode* start = getProperNode(cellA);
	PathNode* finish = getProperNode(cellB);
	if (!start || !finish)
		return result;

	auto nodes = getPathNodes(start, finish, ignoreLastCell);
	if (!nodes.empty())
		nodes.pop_back();
	if (!nodes.empty())
		nodes.erase(std::begin(nodes));
	for (const auto& node : nodes)
		result.push_back(node->cellIndex);

	std::reverse(std::begin(result), std::end(result));

	return result;
}

std::vector<Pathfinder::PathNode*> Pathfinder::getPathNodes(PathNode* start, PathNode* finish, bool ignoreLastCell)
{
	if (start == finish || !mTiles)
		return {};

	resetLastlyUsedNodes();
	mLastlyUsedNodes.insert(start);
	mLastlyUsedNodes.insert(finish);

	/*for (auto& a : mNodes)
	{
		for (auto& b : a)
			resetNode(b);
	}*/
	auto nodesComparator = [](const PathNode* lhs, const PathNode* rhs)
	{
		return lhs->getFullCost() > rhs->getFullCost();
	};

	std::priority_queue<PathNode*, std::vector<PathNode*>, decltype(nodesComparator)> queue(nodesComparator);
	
	const int moveCost = 1;
	start->localCost = 0;
	start->guessCost = calculateGuessCost(start, finish);
	queue.push(start);

	while (!queue.empty())
	{
		PathNode* current = queue.top();
		queue.pop();
		mLastlyUsedNodes.insert(current);

		if (current == finish)
			return reconstructPath(finish);
		
		current->visited = true;
		auto neighbors = getNeighbors(current, finish, ignoreLastCell);

		for (const auto& neighbor : neighbors)
		{
			if (neighbor->visited)
				continue;

			int newLocalCost = current->localCost + moveCost;
			if (newLocalCost < neighbor->localCost)
			{
				neighbor->localCost = newLocalCost;
				neighbor->guessCost = calculateGuessCost(neighbor, finish);
				neighbor->parent = current;
				queue.push(neighbor);
				mLastlyUsedNodes.insert(neighbor);
			}
		}

	}

	return {};
}

std::vector<Pathfinder::PathNode*> Pathfinder::getNeighbors(const PathNode* node, const PathNode* finish, bool ignoreLastCell)
{
	std::vector<PathNode*> neighbors;
	constexpr std::array<sf::Vector2i, 4> offsets = 
	{ 
		sf::Vector2i(-1, 0), 
		sf::Vector2i(1, 0),
		sf::Vector2i(0, -1), 
		sf::Vector2i(0, 1) 
	};

	for (const auto& offset : offsets)
	{
		const int x = node->cellIndex.x;
		const int y = node->cellIndex.y;

		const int nx = x + offset.x; //neighborX
		const int ny = y + offset.y; //neighbory
		const sf::Vector2i index{ nx, ny };

		bool indexValid = isIndexValid(index);
		if (!indexValid)
			continue;
		PathNode* neighborNode = &mNodes[ny][nx];
		bool isWalkable = isNodeWalkable(neighborNode);
		if (neighborNode == finish)
		{
			if (!ignoreLastCell && isNodeWalkableRaw(neighborNode))
				neighbors.push_back(neighborNode);
			else if (ignoreLastCell || isWalkable)
				neighbors.push_back(neighborNode);
			continue;
		}
		if (isWalkable)
			neighbors.push_back(neighborNode);
	}
	return neighbors;
}

bool Pathfinder::isIndexValid(const sf::Vector2i& index) const
{
	return (index.x >= 0 && index.y >= 0 && index.y < mNodes.size() && index.x < mNodes[index.y].size());
}

bool Pathfinder::isNodeWalkable(const sf::Vector2i& index) const
{
	return (*mTiles)[index.y][index.x].isWalkable();
	//return (*mTiles)[index.y][index.x].tileType != TileType::Wall;
}

bool Pathfinder::isNodeWalkable(const PathNode* node) const
{
	return isNodeWalkable(node->cellIndex);
}

bool Pathfinder::isNodeWalkableRaw(const PathNode* node) const
{
	return (*mTiles)[node->cellIndex.y][node->cellIndex.x].isWalkableRaw();
}

int Pathfinder::calculateGuessCost(const PathNode* from, const PathNode* to) const
{
	return Utilities::getDistanceBetweenCells(from->cellIndex, to->cellIndex);
	//return std::abs(from->cellIndex.x - to->cellIndex.x) + std::abs(from->cellIndex.y - to->cellIndex.y);
}

void Pathfinder::resetNode(PathNode& node)
{
	node.parent = nullptr;
	node.localCost = 1500;
	node.guessCost = 0;
	node.visited = false;
}

void Pathfinder::resetLastlyUsedNodes()
{
	for (auto& node : mLastlyUsedNodes)
		resetNode(*node);
	mLastlyUsedNodes.clear();
}

std::vector<Pathfinder::PathNode*> Pathfinder::reconstructPath(PathNode* finish)
{
	std::vector<PathNode*> result;
	PathNode* current = finish;
	while (current != nullptr)
	{
		result.push_back(current);
		current = current->parent;
	}
	return result;
}
