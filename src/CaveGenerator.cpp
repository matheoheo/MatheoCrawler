#include "pch.h"
#include "CaveGenerator.h"
#include "Random.h"

IMapGenerator::GeneratedMap CaveGenerator::generate(const sf::Vector2i& size, int maxDepth, const sf::Vector2i& minRoomSize, const sf::Vector2i& maxRoomSize)
{
	mSpawnPoints.clear();
	IMapGenerator::GeneratedMap map;
	initalize(map, size);
	setMapBordersAsWall(map);
	populateWallsRandomly(map);
	generateCA(map);
	connectCaves(map);
	return map;
}

void CaveGenerator::initalize(GeneratedMap& map, const sf::Vector2i& size)
{
	map = GeneratedMap(size.y, std::vector<TileType>(size.x, TileType::Floor));
}

void CaveGenerator::setMapBordersAsWall(GeneratedMap& map)
{
	constexpr TileType wall = TileType::Wall;
	for (size_t i = 0; i < map.size(); ++i)
	{
		map[i][0] = wall;
		map[i][map[0].size() - 1] = wall;
	}

	for (size_t i = 0; i < map[0].size(); ++i)
	{
		map[0][i] = wall;
		map[map.size() - 1][i] = wall;
	}
}

void CaveGenerator::populateWallsRandomly(GeneratedMap& map)
{
	constexpr TileType wallTile = TileType::Wall;
	constexpr int minFillPercentage = 40;
	constexpr int maxFillPercentage = 50;
	const int fillPercentage = Random::get(minFillPercentage, maxFillPercentage);
	const size_t innerHeight = map.size() - 2; //subtracting 2, because we wanna skip borders
	const size_t innerWidth = map[0].size() - 2;
	const size_t totalCells = innerHeight * innerWidth;

	std::vector<TileType*> tmpMap;
	tmpMap.reserve(totalCells);
	
	//Borders of the map are already set as walls, so we skip them.
	for (size_t y = 1; y < map.size() - 1; ++y)
	{
		for (auto x = 1; x < map[y].size() - 1; ++ x)
			tmpMap.push_back(&map[y][x]);
	}
	std::ranges::shuffle(tmpMap, Random::mt);
	const size_t wallsToPlace = totalCells * fillPercentage / 100;

	for (size_t i = 0; i < wallsToPlace; ++i)
		*tmpMap[i] = wallTile;
}

void CaveGenerator::performCARound(GeneratedMap& map)
{
	//In each iteration we gonna we be determining if given tile is suppoused to be a Floor or Wall
	//But we do not want to falsificate results by updating map also every time
	//That's why we must copy map, and actualize original map only after all iterations have been finished
	IMapGenerator::GeneratedMap tmpMap = map;
	constexpr int countToBecomeFloor = 4;
	constexpr int countToBecomeWall = 5;

	for (size_t y = 1; y < tmpMap.size() - 1; ++y)
	{
		for (size_t x = 1; x < tmpMap[y].size() - 1; ++x)
		{
			auto& type = tmpMap[y][x];
			int count = getWallCount(map, x, y);
			if (type == TileType::Wall && count < countToBecomeFloor)
				type = TileType::Floor;
			else if (type == TileType::Floor && count >= countToBecomeWall)
				type = TileType::Wall;
		}
	}

	map = tmpMap;
}

void CaveGenerator::generateCA(GeneratedMap& map)
{
	constexpr int minRounds = 4;
	constexpr int maxRounds = 8;
	const int caRounds = Random::get(minRounds, maxRounds);
	for (int i = 0; i < caRounds; ++i)
		performCARound(map);
}

std::vector<std::vector<sf::Vector2i>> CaveGenerator::findCaves(GeneratedMap& map)
{
	std::vector<std::vector<sf::Vector2i>> caves;
	std::vector<std::vector<bool>> visited(map.size(), std::vector<bool>(map[0].size(), false));

	for (size_t y = 1; y < map.size() - 1; ++y)
	{
		for (size_t x = 1; x < map[y].size() - 1; ++x)
		{
			if (map[y][x] == TileType::Floor && !visited[y][x])
			{
				caves.push_back(floodFill(map, visited, x, y));
			}
		}
	}
	return caves;
}

std::vector<sf::Vector2i> CaveGenerator::floodFill(const GeneratedMap& map, std::vector<std::vector<bool>>& visited, int x, int y)
{
	constexpr std::array<sf::Vector2i, 4> offsets =
	{ {
		{0, 1}, {0, -1}, {1, 0}, {-1, 0}
	} };
	std::vector<sf::Vector2i> cave;
	std::stack<sf::Vector2i> stack;
	visited[y][x] = true;
	stack.push({ x, y });
	
	while (!stack.empty())
	{
		sf::Vector2i current = stack.top();
		stack.pop();
		cave.push_back(current);
			
		for (const auto& offset : offsets)
		{
			int nx = current.x + offset.x;
			int ny = current.y + offset.y;
			if (map[ny][nx] == TileType::Floor && !visited[ny][nx])
			{
				visited[ny][nx] = true;
				stack.push({ nx, ny });
			}
		}
	}

	return cave;
}

void CaveGenerator::connectCaves(GeneratedMap& generatedMap)
{
	auto caves = findCaves(generatedMap);

	auto biggestCaveIt = std::ranges::max_element(caves, [](const auto& lhs, const auto& rhs) {
		return lhs.size() < rhs.size();
		});

	auto& biggestCave = *biggestCaveIt;

	for (auto& cave : caves)
	{
		if (&cave == &biggestCave)
			continue;

		int bestDist = std::numeric_limits<int>::max();
		sf::Vector2i bestA, bestB;

		for (auto& a : cave)
		{
			for (auto& b : biggestCave)
			{
				int dist = std::abs(a.x - b.x) + std::abs(a.y - b.y);
				if (dist < bestDist)
				{
					bestDist = dist;
					bestA = a;
					bestB = b;
				}
			}
		}
		carveTunnel(generatedMap, bestA, bestB);
		biggestCave.insert(std::end(biggestCave), std::begin(cave), std::end(cave));
	}
}

int CaveGenerator::getWallCount(const GeneratedMap& map, int x, int y) const
{
	int count = 0;
	for (int i = y - 1; i <= y + 1; ++i)
	{
		for (int j = x - 1; j <= x + 1; ++j)
		{
			if (i == y && j == x) //must skip middle point
				continue;
			if (map[i][j] == TileType::Wall)
				++count;
		}
	}

	return count;
}
