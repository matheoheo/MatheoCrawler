#include "pch.h"
#include "CaveGenerator.h"
#include "Random.h"
#include "Config.h"

IMapGenerator::GeneratedMap CaveGenerator::generate(const sf::Vector2i& size, int maxDepth, const sf::Vector2i& minRoomSize, const sf::Vector2i& maxRoomSize)
{
	IMapGenerator::GeneratedMap map;
	initialize(map, size);
	setMapBordersAsWall(map);
	populateWallsRandomly(map);
	generateCA(map);
	connectCaves(map);
	setSpawnCell(map);
	setNextLevelCell(map);
	populateSpawnPoints(map);
	removeHiddenWalls(map);
	return map;
}

void CaveGenerator::initialize(GeneratedMap& map, const sf::Vector2i& size)
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
			if (type == TileType::Floor && count >= countToBecomeWall)
				type = TileType::Wall;
			else if (type == TileType::Wall && count < countToBecomeFloor)
				type = TileType::Floor;
		}
	}

	map = tmpMap;
}

void CaveGenerator::generateCA(GeneratedMap& map)
{
	constexpr int minRounds = 4;
	constexpr int maxRounds = 7;
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
	constexpr size_t minCaveSize = 15;
	auto caves = findCaves(generatedMap);

	for (auto& cave : caves)
	{
		if (cave.size() < minCaveSize)
		{
			for (auto& cell : cave)
				generatedMap[cell.y][cell.x] = TileType::Wall;
		}
	}

	std::erase_if(caves, [minCaveSize](const auto& cave) {
		return cave.size() < minCaveSize;
		});

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

void CaveGenerator::populateSpawnPoints(const GeneratedMap& map)
{
	mSpawnPoints.clear();
	auto floorCells = getFloorCells(map);
	constexpr int diffMultiplier = 15;
	const int possibleEnemiesCount = static_cast<int>((floorCells.size() * 0.1f)) + Config::difficulityLevel * diffMultiplier;
	const int maxEnemiesCount = static_cast<int>(floorCells.size() / 3);
	const int enemiesCount = std::min(possibleEnemiesCount, maxEnemiesCount);
	std::ranges::shuffle(floorCells, Random::mt);

	for (int i = 0; i < enemiesCount; ++i)
		mSpawnPoints.push_back(floorCells[i]);

	std::cout << "Enemies count: " << enemiesCount << '\n';
}

void CaveGenerator::removeHiddenWalls(GeneratedMap& map)
{
	//Visibility system will never let render walls, that are surrounded only by other walls.
	//That means, if there are walls like this, we can set them to TileType::None, that will save our compiutioning time.
	IMapGenerator::GeneratedMap tmpMap = map;
	for (size_t y = 1; y < map.size() - 1; ++y)
	{
		for (size_t x = 1; x < map[y].size() - 1; ++x)
		{
			if (!isAWall(map[y][x]))
				continue;

			int wallsCount = getWallCount(map, x, y);
			if (wallsCount == 8)
				tmpMap[y][x] = TileType::None;
		}
	}
	map = tmpMap;
}

std::vector<sf::Vector2i> CaveGenerator::getFloorCells(const GeneratedMap& map) const
{
	constexpr int minDistFromSpawnCell = 10;
	std::vector<sf::Vector2i> floorCells;
	for (size_t y = 1; y < map.size() - 1; ++y)
	{
		for (size_t x = 1; x < map[y].size() - 1; ++x)
		{
			auto& cell = map[y][x];
			if (std::abs(static_cast<int>(y) - mSpawnCell.y) < minDistFromSpawnCell || std::abs(static_cast<int>(x) - mSpawnCell.x) < minDistFromSpawnCell)
				continue; //skip cells that are too close to player's spawn position.

			if (isAFloor(cell))
				floorCells.push_back({ static_cast<int>(x), static_cast<int>(y) });
		}
	}
	return floorCells;
}

void CaveGenerator::setSpawnCell(const GeneratedMap& map)
{
	constexpr int min = 1;
	const int maxY = map.size() - 2;
	const int maxX = map[0].size() - 2;
	int x = 0;
	int y = 0;

	do
	{
		x = Random::get(min, maxX);
		y = Random::get(min, maxY);

	} while (!isAFloor(map[y][x]));

	mSpawnCell = { x, y };
}

void CaveGenerator::setNextLevelCell(const GeneratedMap& map)
{
	//Finds the most furthest away cell from startPoint based on manhattanDistance
	int maxDistance = std::numeric_limits<int>::min();
	
	for (size_t y = 1; y < map.size() - 1; ++y)
	{
		for (size_t x = 1; x < map[y].size() - 1; ++x)
		{
			if (!isAFloor(map[y][x]))
				continue;

			int distance = std::abs(static_cast<int>(y) - mSpawnCell.y) + std::abs(static_cast<int>(x) - mSpawnCell.x);
			if (distance > maxDistance)
			{
				maxDistance = distance;
				mNextLevelCell = { static_cast<int>(x), static_cast<int>(y) };
			}
		}
	}

}
