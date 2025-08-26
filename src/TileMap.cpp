#include "pch.h"
#include "TileMap.h"
#include "Config.h"
#include "Utilities.h"
#include "Pathfinder.h"
#include "CaveGenerator.h"

TileMap::TileMap(EventManager& eventManager, const sf::View& gameView, Pathfinder& pathfinder)
	:mGameView(gameView),
	mPathfinder(pathfinder)
{
	registerToEvents(eventManager);
}
void TileMap::buildFromGenerator(CaveGenerator& gen, const sf::Vector2i& size, int maxDepth, const sf::Vector2i& minRoomSize, const sf::Vector2i& maxRoomSize)
{
	auto map = std::move(gen.generate(size, maxDepth, minRoomSize, maxRoomSize));
	createTiles(map);
}

void TileMap::render(sf::RenderWindow& window) const
{
	/*for (const auto& row : mTiles)
	{
		for (const auto& tile : row)
		{
			if (tile.tileType == TileType::None) 
				continue;
			window.draw(tile.tileVisual);
		}
	}*/
	renderVisibleTiles(window);
}

bool TileMap::isTileWalkable(const Tile* tile) const
{
	if(!tile)
		return false;

	return tile->isWalkable();
}

bool TileMap::isTileWalkable(int x, int y) const
{
	if(!isInMapBounds(x, y))
		return false;

	return isTileWalkable(&mTiles[y][x]);
}

bool TileMap::isTileWalkable(const sf::Vector2f& tilePos) const
{
	auto index = Utilities::getCellIndex(tilePos);
	return isTileWalkable(index.x, index.y);
}

bool TileMap::isTileOccupied(int x, int y) const
{
	if(!isInMapBounds(x, y))
		return false;
	return !mTiles[y][x].occupyingEntities.empty();
}

bool TileMap::isBlockingSight(int x, int y) const
{
	if(!isInMapBounds(x, y))
		return false;

	return mTiles[y][x].tileType == TileType::Wall;
}

bool TileMap::isLineOfSightClear(const sf::Vector2i& fromCell, const sf::Vector2i& toCell) const
{
	if (fromCell == toCell)
		return true;

	int dx = std::abs(fromCell.x - toCell.x); //deltaX
	int dy = std::abs(fromCell.y - toCell.y); //deltaY
	int dirX = (fromCell.x < toCell.x) ? 1 : -1; //move left or right
	int dirY = (fromCell.y < toCell.y) ? 1 : -1; //move up or down

	int err = dx - dy; //deflection is more in X or Y axis

	sf::Vector2i current = fromCell;
	while (current != toCell)
	{
		if (current != fromCell && isBlockingSight(current.x, current.y))
			return false;

		int e2 = err * 2;

		if (e2 > -dy)
		{
			err -= dy;
			current.x += dirX;
		}
		if (e2 < dx)
		{
			err += dx;
			current.y += dirY;
		}
	}
	return true;
}

bool TileMap::isTileSolid(int x, int y) const
{
	if (!isInMapBounds(x, y))
		return true;

	return !mTiles[y][x].isWalkableRaw();
}

sf::Vector2f TileMap::getFirstWalkablePos() const
{
	for (const auto& row : mTiles)
	{
		for (const auto& tile : row)
		{
			if (isTileWalkable(&tile))
				return tile.tileVisual.getPosition();
		}
	}
	return {};
}

std::vector<std::vector<Tile>>& TileMap::getTiles() 
{
	return mTiles;
}

const std::vector<std::vector<Tile>>& TileMap::getTiles() const
{
	return mTiles;
}

std::vector<Tile*>& TileMap::getVisibleTiles()
{
	return mVisibleTiles;
}

const std::vector<Tile*>& TileMap::getVisibleTiles() const
{
	return mVisibleTiles;
}

std::vector<Tile*>& TileMap::getTilesInCameraBounds()
{
	return mTilesInCameraBounds;
}

bool TileMap::isInMapBounds(int x, int y) const
{
	return y >= 0 && x >= 0 && y < mTiles.size() && x < mTiles[y].size();
}

bool TileMap::isInMapBounds(const sf::Vector2f& pos) const
{
	auto index = Utilities::getCellIndex(pos);
	return isInMapBounds(index.x, index.y);
}

bool TileMap::doesPathExist(const Entity& from, const Entity& to)
{
	auto fromCell = Utilities::getEntityCell(from);
	auto toCell = Utilities::getEntityCell(to);
	
	return doesPathExist(fromCell, toCell);
}

bool TileMap::doesPathExist(const sf::Vector2i& from, const sf::Vector2i& to) const
{
	auto path = mPathfinder.getPath(from, to);
	return !path.empty();
}

std::vector<Entity*>& TileMap::getVisibleEntities()
{
	return mVisibleEntities;
}

const std::vector<Entity*>& TileMap::getVisibleEntities() const
{
	return mVisibleEntities;
}

std::vector<Entity*> TileMap::getEntitiesOnTile(int x, int y) const
{
	if (!isInMapBounds(x, y))
		return {};

	const auto& tile = &mTiles[y][x];
	return tile->occupyingEntities;
}

std::vector<Entity*> TileMap::getEntitiesOnTile(const Tile& tile) const
{
	return tile.occupyingEntities;
}

const Tile* TileMap::getTile(int x, int y) const
{
	if(!isInMapBounds(x, y))
		return nullptr;
	
	return &mTiles[y][x];
}

const Tile* TileMap::getTile(const sf::Vector2f& pos) const
{
	auto cell = Utilities::getCellIndex(pos);
	return getTile(cell.x, cell.y);
}

void TileMap::createTiles(const IMapGenerator::GeneratedMap& map)
{
	const size_t mapHeight = map.size();
	const size_t mapWidth = map[0].size();

	mTiles = std::vector(mapHeight, std::vector<Tile>(mapWidth));

	for (size_t y = 0; y < mapHeight; ++y)
	{
		for (size_t x = 0; x < mapWidth; ++x)
		{
			sf::Vector2f position{x * Config::getCellSize().x, y * Config::getCellSize().y};
			mTiles[y][x] = std::move(Tile(position, map[y][x]));
			
		}
	}
}

void TileMap::renderVisibleTiles(sf::RenderWindow& window) const
{
	for (const auto& tile : mVisibleTiles)
	{
		window.draw(tile->tileVisual);
	}

}

void TileMap::registerToEvents(EventManager& eventManager)
{
	registerToReserveTileEvent(eventManager);
	registerToTileOccupiedEvent(eventManager);
	registerToTileVacatedEvent(eventManager);
	registerToRemoveEntityFromSystemEvent(eventManager);
	registerToBeforeLoadNextLevelEvent(eventManager);
}

void TileMap::registerToReserveTileEvent(EventManager& eventManager)
{
	eventManager.registerEvent<ReserveTileEvent>([this](const ReserveTileEvent& data)
		{
			if (!isInMapBounds(data.tilePos))
				return;

			auto tileIndex = Utilities::getCellIndex(data.tilePos);
			reserveTile(mTiles[tileIndex.y][tileIndex.x], data.entity);
		});
}

void TileMap::registerToTileOccupiedEvent(EventManager& eventManager)
{
	eventManager.registerEvent<TileOccupiedEvent>([this](const TileOccupiedEvent& data)
		{
			if (!isInMapBounds(data.tilePos))
				return;

			auto tileIndex = Utilities::getCellIndex(data.tilePos);
			occupyTile(mTiles[tileIndex.y][tileIndex.x], data.entity);
		});
}

void TileMap::registerToTileVacatedEvent(EventManager& eventManager)
{
	eventManager.registerEvent<TileVacatedEvent>([this](const TileVacatedEvent& data)
		{
			if (!isInMapBounds(data.tilePos))
				return;

			auto tileIndex = Utilities::getCellIndex(data.tilePos);
			vacateTile(mTiles[tileIndex.y][tileIndex.x], data.entity);
		});
}

void TileMap::reserveTile(Tile& tile, Entity* entity)
{
	tile.reservedEntity = entity;
}

void TileMap::occupyTile(Tile& tile, Entity& entity)
{
	tile.occupyingEntities.push_back(&entity);
}

void TileMap::vacateTile(Tile& tile, Entity& entity)
{
	std::erase(tile.occupyingEntities, &entity);
}

void TileMap::registerToRemoveEntityFromSystemEvent(EventManager& eventManager)
{
	eventManager.registerEvent<RemoveEntityFromSystemEvent>([this](const RemoveEntityFromSystemEvent& data)
		{
			std::erase(mVisibleEntities, &data.entity);
		});
}

void TileMap::registerToBeforeLoadNextLevelEvent(EventManager& eventManager)
{
	eventManager.registerEvent<BeforeLoadNextLevelEvent>([this](const BeforeLoadNextLevelEvent& data)
		{
			mVisibleTiles.clear();
			mVisibleEntities.clear();
			mTilesInCameraBounds.clear();
		});
}

