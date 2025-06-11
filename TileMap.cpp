#include "pch.h"
#include "TileMap.h"
#include "Config.h"
#include "Utilities.h"
#include "Pathfinder.h"

TileMap::TileMap(EventManager& eventManager, const sf::View& gameView, Pathfinder& pathfinder)
	:mGameView(gameView),
	mPathfinder(pathfinder)
{
	registerToEvents(eventManager);
}
void TileMap::buildFromGenerator(DungeonGenerator& gen, const sf::Vector2i& size, int maxDepth, const sf::Vector2i& minRoomSize, const sf::Vector2i& maxRoomSize)
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

void TileMap::calculateVisibleTiles(const sf::Vector2f& centerPosition)
{
	if (mTiles.empty())
		return;

	mVisibleTiles.clear();

	const auto viewSize = mGameView.getSize();
	const auto cellSize = Config::getCellSize();
	const sf::Vector2f halfView(viewSize * 0.5f);

	const sf::Vector2i cellsInHalf(static_cast<int>(halfView.x / cellSize.x), 
		static_cast<int>(halfView.y / cellSize.y));
	const sf::Vector2i myCell = Utilities::getCellIndex(centerPosition);
	const int margin = 2; //how many more tiles outside view render
	const int mapHeight = static_cast<int>(mTiles.size());
	const int mapWidth = static_cast<int>(mTiles[0].size());

	int minCellX = std::max(0, myCell.x - cellsInHalf.x - margin);
	int maxCellX = std::min(myCell.x + cellsInHalf.x + margin, mapWidth);

	int minCellY = std::max(0, myCell.y - cellsInHalf.y - margin);
	int maxCellY = std::min(myCell.y + cellsInHalf.y + margin, mapHeight);

	for (int y = minCellY; y < maxCellY; ++y)
	{
		for (int x = minCellX; x < maxCellX; ++x)
		{
			if (mTiles[y][x].tileType == TileType::None)
				continue;
			mVisibleTiles.push_back(&mTiles[y][x]);
		}
	}
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
	auto path = mPathfinder.getPath(fromCell, toCell, false);

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

