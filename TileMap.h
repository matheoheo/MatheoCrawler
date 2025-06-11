#pragma once
#include "Tile.h"
#include "DungeonGenerator.h"
#include "EventManager.h"

class Pathfinder;

class TileMap
{
public:
	TileMap(EventManager& eventManager, const sf::View& gameView, Pathfinder& pathfinder);
	void buildFromGenerator(DungeonGenerator& gen, const sf::Vector2i& size, int maxDepth,
		const sf::Vector2i& minRoomSize, const sf::Vector2i& maxRoomSize);

	void render(sf::RenderWindow& window) const;
	void calculateVisibleTiles(const sf::Vector2f& centerPosition);

	bool isTileWalkable(const Tile* tile) const;
	bool isTileWalkable(int x, int y) const;
	bool isTileWalkable(const sf::Vector2f& tilePos) const;
	bool isBlockingSight(int x, int y) const;
	bool isLineOfSightClear(const sf::Vector2i& fromCell, const sf::Vector2i& toCell) const;
	sf::Vector2f getFirstWalkablePos() const;

	std::vector<std::vector<Tile>>& getTiles();
	const std::vector<std::vector<Tile>>& getTiles() const;
	std::vector<Tile*>& getVisibleTiles();
	const std::vector<Tile*>& getVisibleTiles() const;
	std::vector<Tile*>& getTilesInCameraBounds();

	bool isInMapBounds(int x, int y) const;
	bool isInMapBounds(const sf::Vector2f& pos) const;
	bool doesPathExist(const Entity& from, const Entity& to);

	std::vector<Entity*>& getVisibleEntities();
	const std::vector<Entity*>& getVisibleEntities() const;

	std::vector<Entity*> getEntitiesOnTile(int x, int y) const;
private:
	void createTiles(const IMapGenerator::GeneratedMap& map);
	void renderVisibleTiles(sf::RenderWindow& window) const;
	void registerToEvents(EventManager& eventManager);
	void registerToReserveTileEvent(EventManager& eventManager);
	void registerToTileOccupiedEvent(EventManager& eventManager);
	void registerToTileVacatedEvent(EventManager& eventManager);

	void reserveTile(Tile& tile, Entity* entity);
	void occupyTile(Tile& tile, Entity& entity);
	void vacateTile(Tile& tile, Entity& entity);

private:
	const sf::View& mGameView;
	Pathfinder& mPathfinder;
	std::vector<std::vector<Tile>> mTiles;
	std::vector<Tile*> mVisibleTiles;
	std::vector<Entity*> mVisibleEntities;
	std::vector<Tile*> mTilesInCameraBounds;
};

