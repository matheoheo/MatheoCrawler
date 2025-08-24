#include "pch.h"
#include "DungeonGenerator.h"
#include "Random.h"
#include "Utilities.h"
#include "Config.h"

IMapGenerator::GeneratedMap DungeonGenerator::generate(const sf::Vector2i& size, int maxDepth, const sf::Vector2i& minRoomSize, const sf::Vector2i& maxRoomSize)
{
    mSpawnPoints.clear();
    IMapGenerator::GeneratedMap map;
    BSPNode rootNode({});
    initalize(map, rootNode, size);
    runAlgorithm(rootNode, minRoomSize, maxRoomSize);
    collectLeafNodes(rootNode);
    createRooms(minRoomSize);
    placeRoomsOnMap(map);
    sortRooms();
    generateObstacles(map);
    createRoomConnections(map);
    addWalls(map);
    populateSpawnPoints(map);
    setSpawnCell(map);
    setNextLevelCell(map);
    return map;
}

bool DungeonGenerator::isALeaf(const BSPNode& node) const
{
    return node.firstChild == nullptr && node.secondChild == nullptr;
}

void DungeonGenerator::initalize(GeneratedMap& map, BSPNode& rootNode, const sf::Vector2i& size)
{
    map = GeneratedMap(size.y, std::vector<TileType>(size.x, TileType::None));
    rootNode.area.position = { 0, 0 };
    rootNode.area.size = size;
    mLeafNodes.clear();
    mRooms.clear();
}

void DungeonGenerator::runAlgorithm(BSPNode& node, const sf::Vector2i& minRoomSize, const sf::Vector2i& maxRoomSize)
{
    if (!isALeaf(node))
        return;

    if (node.area.size.x > maxRoomSize.x ||
        node.area.size.y > maxRoomSize.y ||
        Random::get(1, 100) > 25)
    {
        if (divideNode(node, minRoomSize))
        {
            runAlgorithm(*node.firstChild,  minRoomSize, maxRoomSize);
            runAlgorithm(*node.secondChild, minRoomSize, maxRoomSize);
        }
    }

}

bool DungeonGenerator::divideNode(BSPNode& node, const sf::Vector2i& minRoomSize)
{
    if (!isALeaf(node))
        return false;

    bool splitVertically = false;
    const float fWidth = static_cast<float>(node.area.size.x);
    const float fHeight = static_cast<float>(node.area.size.y);
    const float dimensionProportion = 1.25f;

    if (fWidth / fHeight >= dimensionProportion)
        splitVertically = true;
    else if (fHeight / fWidth >= dimensionProportion)
        splitVertically = false;
    else
        splitVertically = Random::get(1, 10) > 5;

    if (splitVertically && node.area.size.x < minRoomSize.x * 2 || !splitVertically && node.area.size.y < minRoomSize.y * 2)
        return false;

    int splitPoint = (splitVertically) ?
        Random::get(minRoomSize.x, node.area.size.x - minRoomSize.x) :
        Random::get(minRoomSize.y, node.area.size.y - minRoomSize.y);

    if (splitVertically)
    {
        node.firstChild =  createNode(sf::IntRect(node.area.position, {splitPoint, node.area.size.y}));
        node.secondChild = createNode(sf::IntRect({ node.area.position.x + splitPoint, node.area.position.y }, { node.area.size.x - splitPoint, node.area.size.y }));
    }
    else
    {
        node.firstChild = createNode(sf::IntRect(node.area.position, { node.area.size.x, splitPoint }));
        node.secondChild = createNode(sf::IntRect({ node.area.position.x, node.area.position.y + splitPoint },
            { node.area.size.x, node.area.size.y - splitPoint }));
    }


    return true;
}

std::unique_ptr<DungeonGenerator::BSPNode> DungeonGenerator::createNode(const sf::IntRect& rect) const
{
    return std::make_unique<BSPNode>(rect);
}

void DungeonGenerator::collectLeafNodes(BSPNode& node)
{
    if (isALeaf(node))
        mLeafNodes.push_back(&node);
    else
    {
        collectLeafNodes(*node.firstChild);
        collectLeafNodes(*node.secondChild);
    }

}

void DungeonGenerator::createRooms(const sf::Vector2i& minRoomSize)
{
    const int margin = 2;
    for (const auto& leaf : mLeafNodes)
    {
        const int maxWidth = leaf->area.size.x - margin;
        const int maxHeight = leaf->area.size.y - margin;

        if (minRoomSize.x > maxWidth || minRoomSize.y > maxHeight)
            continue;

        const int roomWidth = Random::get(minRoomSize.x, maxWidth);
        const int roomHeight = Random::get(minRoomSize.y, maxHeight);

        const int roomX = Random::get(1, std::max(0, leaf->area.size.x - roomWidth - 1)) + leaf->area.position.x;
        const int roomY = Random::get(1, std::max(0, leaf->area.size.y - roomHeight - 1)) + leaf->area.position.y;

        mRooms.emplace_back(sf::IntRect({ roomX, roomY }, { roomWidth, roomHeight }));
    }
    std::cout << "Rooms amount is: " << mRooms.size() << '\n';
}

void DungeonGenerator::placeRoomsOnMap(GeneratedMap& map)
{
    std::cout << "Rooms amount is: " << mRooms.size() << '\n';

    for (const auto& room : mRooms)
    {
        const auto roomPos = room.area.position;
        const auto roomSize = room.area.size;

        for (int y = roomPos.y; y < roomPos.y + roomSize.y; ++y)
        {
            for (int x = roomPos.x; x < roomPos.x + roomSize.x; ++x)
            {
                map[y][x] = TileType::Floor;
            }
        }

    }
}

sf::Vector2i DungeonGenerator::getRoomCenter(const Room& room) const
{
    return { room.area.position.x + room.area.size.x / 2,
             room.area.position.y + room.area.size.y / 2 
           };
}

void DungeonGenerator::createRoomConnections(GeneratedMap& map)
{
    if (mRooms.empty())
        return;

    //this vector holds distances between rooms
    std::vector<std::tuple<const Room*, const Room*, float>> edges;
    for (size_t i = 0; i < mRooms.size(); ++i)
    {
        for (size_t j = i + 1; j < mRooms.size(); ++j)
        {
            const auto& roomA = mRooms[i];
            const auto& roomB = mRooms[j];
            float distance = Utilities::getDistanceBetween(getRoomCenter(roomA), getRoomCenter(roomB));
            edges.push_back(std::make_tuple(&roomA, &roomB, distance));
        }
    }

    //need to sort the vector in ascending order
    std::sort(std::begin(edges), std::end(edges), [](const auto& edge1, const auto& edge2)
        {
            return std::get<2>(edge1) < std::get<2>(edge2);
        });

    std::cout << "Edges size is: " << edges.size() << std::endl;

    std::unordered_set<const Room*> roomsConnected;
    std::unordered_set<const Room*> remainingRooms;

    for (const auto& room : mRooms)
        remainingRooms.insert(&room);

    const Room* firstRoom = &mRooms[0];

    roomsConnected.insert(firstRoom);
    remainingRooms.erase(firstRoom);

    auto makeConnection = [this, &roomsConnected, &remainingRooms, &map](const Room& roomConnected, const Room& roomToConnect)
    {
        remainingRooms.erase(&roomToConnect);
        roomsConnected.insert(&roomToConnect);
        createCorridorBetween(map, roomConnected, roomToConnect);
    };

    while (!remainingRooms.empty())
    {
        for (auto it = std::begin(edges); it != std::end(edges);)
        {
            const Room* roomA = std::get<0>(*it);
            const Room* roomB = std::get<1>(*it);
            const float distance = std::get<2>(*it);

            bool roomAIsConnected = roomsConnected.contains(roomA);
            bool roomBIsConnected = roomsConnected.contains(roomB);

            if (roomAIsConnected && !roomBIsConnected)
            {
                makeConnection(*roomA, *roomB);
                it = edges.erase(it);
                break;
            }
            else if (!roomAIsConnected && roomBIsConnected)
            {
                makeConnection(*roomB, *roomA);
                it = edges.erase(it);
                break;
            }
            else
                ++it;
        }
    }
    std::cout << "Edges size after is: " << edges.size() << std::endl;
}

void DungeonGenerator::createCorridorBetween(GeneratedMap& map, const Room& roomA, const Room& roomB)
{
    auto pointA = getRoomCenter(roomA);
    auto pointB = getRoomCenter(roomB);
    carveTunnel(map, pointA, pointB);
}

void DungeonGenerator::addWalls(GeneratedMap& map)
{
    const int height = static_cast<int>(map.size());
    const int width  = static_cast<int>(map[0].size());

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (map[y][x] != TileType::None)
                continue;

            bool nearFloor = false;

            for (int dy = -1; dy <= 1 && !nearFloor; ++dy)
            {
                for (int dx = -1; dx <= 1; ++dx)
                {
                    int ny = y + dy;
                    int nx = x + dx;
                    if (ny >= 0 && ny < height && nx >= 0 && nx < width && map[ny][nx] == TileType::Floor)
                    {
                        nearFloor = true;
                        break;
                    }
                }
                
            }

            if (nearFloor)
                map[y][x] = TileType::Wall;
        }
    }
}

int DungeonGenerator::getRoomArea(const Room& room) const
{
    return room.area.size.x * room.area.size.y;
}

void DungeonGenerator::populateSpawnPoints(GeneratedMap& map)
{
    int it = 0;
    for (const auto& room : mRooms)
    {
        if (it++ == 0)
            continue; //skip first room, because that's where player respawns

        auto walkTilesPos = getWalkableTilesPosInRoom(map, room);
        auto monstersAmount = getMonsterCountInRoom(static_cast<int>(walkTilesPos.size()));

        std::ranges::shuffle(walkTilesPos, Random::mt);

        for (int i = 0; i < monstersAmount && i < walkTilesPos.size(); ++i)
        {
            auto pos = walkTilesPos[i];
            mSpawnPoints.emplace_back(pos.x, pos.y);
        }
    }
}

std::vector<sf::Vector2i> DungeonGenerator::getWalkableTilesPosInRoom(GeneratedMap& map, const Room& room) const
{
    auto isWalkable = [](TileType type) { return type == TileType::Floor; };

    const auto& roomSize = room.area.size;
    const auto& roomPos = room.area.position;

    int minX = roomPos.x;
    int minY = roomPos.y;
    int maxX = minX + roomSize.x;
    int maxY = minY + roomSize.y;
    std::vector<sf::Vector2i> result;
    
    for (int y = minY; y < maxY; ++y)
    {
        for (int x = minX; x < maxX; ++x)
        {
            if (isWalkable(map[y][x]))
                result.push_back({ x, y });
        }
    }

    return result;
}

int DungeonGenerator::getMonsterCountInRoom(int walkableTilesCount)
{
    const float a = 0.05f;
    const float b = 1.2f;
    const int minMonsters = 2;
    const int maxMonsters = walkableTilesCount - 1;

    int count = static_cast<int>(a * walkableTilesCount + b * Config::difficulityLevel + minMonsters);

    if (count > maxMonsters)
        count = maxMonsters;
    else if (count < minMonsters)
        count = minMonsters;

    return count;
}

void DungeonGenerator::sortRooms()
{
    std::sort(std::begin(mRooms), std::end(mRooms), [](const Room& lhs, const Room& rhs)
        {
            if (lhs.area.position.y != rhs.area.position.y)
                return lhs.area.position.y < rhs.area.position.y;
            return lhs.area.position.x < rhs.area.position.x;
        });
}

void DungeonGenerator::setSpawnCell(const GeneratedMap& map)
{
    const auto& firstRoom = mRooms.front();
    const auto& roomPos = firstRoom.area.position;
    const auto& roomSize = firstRoom.area.size;

    for (int y = roomPos.y; y < roomPos.y + roomSize.y; ++y)
    {
        for (int x = roomPos.x; x < roomPos.x + roomSize.x; ++x)
        {
            if (isAFloor(map[y][x]))
            {
                mSpawnCell = { x, y };
                return;
            }
        }
    }
}

void DungeonGenerator::setNextLevelCell(const GeneratedMap& map)
{
    //In this function, we set which tile will have a functionality of upgrading to next level of game.

    //Rooms are sorted - we take lastRoom as our place to have next level advance cell.
    const auto& lastRoom = mRooms.front();
    const auto& roomPos = lastRoom.area.position;
    const auto& roomSize = lastRoom.area.size;

    //We take corner of the room. If it is a floor (means it is walkable) we set it as our next level and return from function. We got our point.
    auto corner = map[roomPos.y][roomPos.x];
    if (isAFloor(corner))
    {
        mNextLevelCell = roomPos;
        return;
    }

    do //Otherwise, we just get random positions inside room till we find one that is a floor.
    {
        mNextLevelCell.x = Random::get(roomPos.x, roomPos.x + roomSize.x - 1);
        mNextLevelCell.y = Random::get(roomPos.y, roomPos.y + roomSize.y - 1);
    } while (!isAFloor(map[mNextLevelCell.y][mNextLevelCell.x]));
}

void DungeonGenerator::generateObstacles(GeneratedMap& map)
{
    //This function generates obstacles inside rooms, based on RNG.
    //Keep in mind, obstacles are generated BEFORE corridors placement.
    //Corridors placement happens between rooms middle points, not edges.
    //This means, that obstacles might be overriden by corridors
    //This way of doing guarantees, that connectivity between rooms is not broken, without using any other pathfinding algorithms.

    const int baseChance = 20;
    const int maxChance = 100;
    const int minRoomArea = 35;

    for (int i = -1; const auto& room : mRooms)
    {
        //skip first room, because player spawns there.
        ++i;
        if (i == 0)
            continue;

        int roomArea = getRoomArea(room);
        if (roomArea < minRoomArea)
            continue;
        
        int chance = std::min(baseChance + roomArea, maxChance);
        //if (Random::get(1, maxChance) > chance)
          //  continue;
        
        placeObstaclePattern(map, room);
    }
}

void DungeonGenerator::placeObstaclePattern(GeneratedMap& map, const Room& room)
{
    std::vector<Pattern> patterns =
    {
        {[this](GeneratedMap& map, const Room& r) {placeLineOfWalls(map, r); }, 60},
        {[this](GeneratedMap& map, const Room& r) {placeRandomSingleWalls(map, r); }, 40},
    };

    int summedWeight = 0;
    for (const auto& p : patterns)
        summedWeight += p.weight;

    int randomWeight = Random::get(0, summedWeight - 1);

    for (const auto& p : patterns)
    {
        if (randomWeight < p.weight)
            return p.obstacleGen(map, room);
        randomWeight -= p.weight;
    }
}

void DungeonGenerator::placeRandomSingleWalls(GeneratedMap& map, const Room& room)
{
    const int area = getRoomArea(room);
    const int divider = 8;
    const int minObstacles = 1;
    const int maxObstacles = area / divider; //maximum of 12% of room area in walls.
    const int obstacleCount = Random::get(minObstacles, maxObstacles);

    //Algorithm works in a way, that i am creating a vector with every second position inside room
    //A bit like chessboard - so it avoids direct neighbours
    //Then shuffle all the possitions and pick first few (up to obstacleCount amount)
    //And place singular walls in this positions.

    std::vector<sf::Vector2i> candidates; //using sf::Vector2i because we are operating on sf::IntRect

    for (int y = room.area.position.y + 1; y < room.area.size.y + room.area.position.y - 1; ++y)
    {
        for (int x = room.area.position.x + 1; x < room.area.size.x + room.area.position.x - 1; ++x)
        {
            if ((x + y) % 2 == 0 && map[y][x] == TileType::Floor)
                candidates.emplace_back(x, y);
        }
    }
    std::ranges::shuffle(candidates, Random::mt);
    int maxCount = std::min(obstacleCount, static_cast<int>(candidates.size()));

    for (int i = 0; i < maxCount; ++i)
    {
        auto pos = candidates[i];
        map[pos.y][pos.x] = TileType::Wall;
    }
}

void DungeonGenerator::placeLineOfWalls(GeneratedMap& map, const Room& room)
{
    bool verticalWall = Random::get(1, 10) > 5;
    const int maxWallLength = (verticalWall ? room.area.size.y : room.area.size.x) - 1;
    const int minWallLength = 4;

    if (minWallLength >= maxWallLength)
        return;

    int actualLength = Random::get(minWallLength, maxWallLength);
    const auto& roomSize = room.area.size;
    const auto& roomPos  = room.area.position;
    const int lengthDiff = (verticalWall ? roomSize.y : roomSize.x) - actualLength;

    if (verticalWall)
    {
        const int x = Random::get(roomPos.x, roomPos.x + roomSize.x - 1);
        int startY = Random::get(roomPos.y, roomPos.y + lengthDiff - 1);

        for (int y = startY; y < startY + actualLength; ++y)
            if(map[y][x] == TileType::Floor)
                map[y][x] = TileType::Wall;
    }
    else
    {
        const int y = Random::get(roomPos.y, roomPos.y + roomSize.y - 1);
        int startX = Random::get(roomPos.x, roomPos.x + lengthDiff - 1);

        for (int x = startX; x < startX + actualLength; ++x)
            if(map[y][x] == TileType::Floor)
                map[y][x] = TileType::Wall;
    }
    

}

