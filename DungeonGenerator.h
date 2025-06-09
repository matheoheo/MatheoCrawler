#pragma once
#include "IMapGenerator.h"
#include <SFML/Graphics.hpp>
//DungeonGenerator based on Binary Space Partitioning algorithm.

class DungeonGenerator :
    public IMapGenerator
{
public:
    // Inherited via IMapGenerator
    virtual GeneratedMap generate(const sf::Vector2i& size, int maxDepth, const sf::Vector2i& minRoomSize, const sf::Vector2i& maxRoomSize) override;
private:
    struct BSPNode
    {
        sf::IntRect area;
        std::unique_ptr<BSPNode> firstChild;
        std::unique_ptr<BSPNode> secondChild;

        explicit BSPNode(const sf::IntRect& area) : area(area) {}
    };
    struct Room
    {
        sf::IntRect area;
        explicit Room(const sf::IntRect& area) : area(area) {}
    };

    bool isALeaf(const BSPNode& node) const;
    void initalize(GeneratedMap& map, BSPNode& rootNode, const sf::Vector2i& size);
    void runAlgorithm(BSPNode& node,   const sf::Vector2i& minRoomSize, const sf::Vector2i& maxRoomSize);
    bool divideNode(BSPNode& node, const sf::Vector2i& minRoomSize);

    std::unique_ptr<BSPNode> createNode(const sf::IntRect& rect) const;
    void collectLeafNodes(BSPNode& node);
    void createRooms(const sf::Vector2i& minRoomSize);
    void placeRoomsOnMap(GeneratedMap& map);
    sf::Vector2i getRoomCenter(const Room& room) const;
    void createRoomConnections(GeneratedMap& map);
    void createCorridorBetween(GeneratedMap& map, const Room& roomA, const Room& roomB);
    void addWalls(GeneratedMap& map);
    void printMap(GeneratedMap& map);
    int getRoomArea(const Room& room) const;
    void populateSpawnPoints(GeneratedMap& map);
    std::vector<sf::Vector2i> getWalkableTilesPosInRoom(GeneratedMap& map, const Room& room) const;
    int getMonsterCountInRoom(int walkableTilesCount);

    void sortRooms();
private:
    //this section is for generating obstacles inside rooms:
    using ObstacleGenerator = std::function<void(GeneratedMap&, const Room&)>;
    struct Pattern 
    {
        ObstacleGenerator obstacleGen;
        int weight;

        Pattern(ObstacleGenerator gen, int weight)
            :obstacleGen(gen), weight(weight) {}
    };

    void generateObstacles(GeneratedMap& map);
    void placeObstaclePattern(GeneratedMap& map, const Room& room);

    void placeRandomSingleWalls(GeneratedMap& map, const Room& room);
    void placeLineOfWalls(GeneratedMap& map, const Room& room);
private:
    std::vector<BSPNode*> mLeafNodes;
    std::vector<Room> mRooms;
    std::unordered_map<const Room*, std::vector<sf::Vector2i>> mEntrances;
};

