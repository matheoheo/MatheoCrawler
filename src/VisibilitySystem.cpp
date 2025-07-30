#include "pch.h"
#include "VisibilitySystem.h"
#include "Config.h"
#include "Utilities.h"

VisibilitySystem::VisibilitySystem(SystemContext& systemContext, TileMap& tileMap, const sf::View& gameView)
	:ISystem(systemContext),
	mTileMap(tileMap),
	mGameView(gameView),
	mShouldRecalculateEntityVisibility(true)
{
	registerToEvents();
	mOctantTransforms =
	{
		{ 1,  0,  0,  1},
		{ 1,  0,  0, -1},
		{-1,  0,  0,  1},
		{-1,  0,  0, -1},
		{ 0,  1,  1,  0},
		{ 0,  1, -1,  0},
		{ 0, -1,  1,  0},
		{ 0, -1, -1,  0}
	};
	
}

void VisibilitySystem::update(const sf::Time& deltaTime)
{
	if (mShouldRecalculateEntityVisibility)
	{
		calculateVisibleEntities();
		mShouldRecalculateEntityVisibility = false;
	}
}

void VisibilitySystem::registerToEvents()
{
	registerToPlayerMoveFinishedEvent();
	registerToMoveAllowedEvent();
	registerToEntityFinishedMoveEvent();
	regToRemoveEntityFromSystemEvent();
	registerToBeforeLoadNextLevelEvent();
}

void VisibilitySystem::registerToPlayerMoveFinishedEvent()
{
	mSystemContext.eventManager.registerEvent<PlayerMoveFinishedEvent>([this](const PlayerMoveFinishedEvent& data)
		{
			computeShadowcastFOV();
			collectTilesInCameraView();
			notifyFadeSystemWithFOVChanges();
			mShouldRecalculateEntityVisibility = true;
			sendUpdateEntityRenderTilesEvent();
		});
}

void VisibilitySystem::registerToMoveAllowedEvent()
{
	mSystemContext.eventManager.registerEvent<MoveAllowedEvent>([this](const MoveAllowedEvent& data)
		{
			if (data.entity.hasComponent<PlayerComponent>())
				return;

			if (isEntityCloseToPlayer(data.entity))
				sendUpdateEntityRenderTilesEvent();
		});
}

void VisibilitySystem::registerToEntityFinishedMoveEvent()
{
	mSystemContext.eventManager.registerEvent<EntityMoveFinishedEvent>([this](const EntityMoveFinishedEvent& data)
		{
			if (isEntityCloseToPlayer(data.entity))
				sendUpdateEntityRenderTilesEvent();
		});
}

void VisibilitySystem::regToRemoveEntityFromSystemEvent()
{
	mSystemContext.eventManager.registerEvent<RemoveEntityFromSystemEvent>([this](const RemoveEntityFromSystemEvent& data)
		{
			mLastVisibleEntities.erase(&data.entity);
		});
}

void VisibilitySystem::registerToBeforeLoadNextLevelEvent()
{
	mSystemContext.eventManager.registerEvent<BeforeLoadNextLevelEvent>([this](const BeforeLoadNextLevelEvent& data)
		{
			std::cout << "Clearing in visibility system\n";
			mTilesInFieldOfView.clear();
			mPreviousFOV.clear();
			mLastVisibleEntities.clear();
		});
}

void VisibilitySystem::collectTilesInCameraView()
{
	auto& tiles = mTileMap.getTiles();
	auto& visibleTiles = mTileMap.getVisibleTiles();
	auto& tilesInCamera = mTileMap.getTilesInCameraBounds();
	auto bounds = getCellBoundsInCameraView();
	visibleTiles.clear();
	tilesInCamera.clear();

	for (int y = bounds.min.y; y < bounds.max.y; ++y)
	{
		for (int x = bounds.min.x; x < bounds.max.x; ++x)
		{
			auto& tile = tiles[y][x];
			if (tile.tileType == TileType::None)
				continue;
			tilesInCamera.push_back(&tile);

			if (tile.visible || tile.explored)
			{
				visibleTiles.push_back(&tile);
			}
		}
	}
}

VisibilitySystem::CellBounds VisibilitySystem::getCellBoundsInCameraView() const
{
	const auto viewSize = mGameView.getSize();
	const auto cellSize = Config::getCellSize();
	const sf::Vector2f halfView(viewSize * 0.5f);

	sf::Vector2i cellsInHalf(static_cast<int>(halfView.x / cellSize.x),
		static_cast<int>(halfView.y / cellSize.y));
	constexpr int margin = 2; //how many more tiles outside view render

	cellsInHalf.x += margin;
	cellsInHalf.y += margin;

	return getCellBoundsInRange(mGameView.getCenter(), cellsInHalf);
}

VisibilitySystem::CellBounds VisibilitySystem::getCellBoundsInRange(const sf::Vector2f& fromPos, const sf::Vector2i& range) const
{
	const auto& tiles = mTileMap.getTiles();
	const auto cellSize = Config::getCellSize();

	const sf::Vector2i myCell = Utilities::getCellIndex(fromPos);
	const int mapHeight = static_cast<int>(tiles.size());
	const int mapWidth = static_cast<int>(tiles[0].size());

	int minCellX = std::max(0, myCell.x - range.x);
	int maxCellX = std::min(myCell.x + range.x, mapWidth);

	int minCellY = std::max(0, myCell.y - range.y);
	int maxCellY = std::min(myCell.y + range.y, mapHeight);

	return CellBounds{ {minCellX, minCellY}, {maxCellX, maxCellY} };
}

void VisibilitySystem::transitionFOVToExplored()
{
	mPreviousFOV.clear();
	for (auto& fovTile : mTilesInFieldOfView)
	{
		fovTile->explored = true;
		fovTile->visible = false;
	}
	mPreviousFOV.insert(std::end(mPreviousFOV), std::begin(mTilesInFieldOfView), std::end(mTilesInFieldOfView));
	mTilesInFieldOfView.clear();
}

void VisibilitySystem::addTileToCurrentFov(Tile& tile)
{
	if (!isValidFOVTile(tile))
		return;

	tile.visible = true;
	mTilesInFieldOfView.push_back(&tile);
}

void VisibilitySystem::addTileToCurrentFov(int x, int y)
{
	if (!mTileMap.isInMapBounds(x, y))
		return;
	
	addTileToCurrentFov(mTileMap.getTiles()[y][x]);
}

bool VisibilitySystem::isValidFOVTile(const Tile& tile)
{
	return tile.tileType != TileType::None || tile.visible;
}

void VisibilitySystem::calculateVisibleEntities()
{
	auto& visibleEntitiesList = mTileMap.getVisibleEntities();
	const auto& visibleTiles = mTileMap.getTilesInCameraBounds();	
	std::unordered_set<Entity*> currentSet;
	for (const auto& tile : visibleTiles)
	{
		if (tile->reservedEntity)
			currentSet.insert(tile->reservedEntity);

		for (auto& ent : tile->occupyingEntities)
			currentSet.insert(ent);
	}

	//detect new apperances
	for (Entity* entity : currentSet)
	{
		if (mLastVisibleEntities.find(entity) == std::end(mLastVisibleEntities))
			notifyMonsterAppeared(*entity);
	}

	//detect monsters that disappeared
	for (Entity* entity : mLastVisibleEntities)
	{
		if (currentSet.find(entity) == std::end(currentSet))
			notifyMonsterDisappeared(*entity);
	}
	mLastVisibleEntities = std::move(currentSet);
	visibleEntitiesList.assign(std::begin(mLastVisibleEntities), std::end(mLastVisibleEntities));
}

void VisibilitySystem::sendUpdateEntityRenderTilesEvent()
{
	mSystemContext.eventManager.notify<UpdateEntityRenderTilesEvent>(UpdateEntityRenderTilesEvent(mTilesInFieldOfView));
}

bool VisibilitySystem::isEntityCloseToPlayer(const Entity& entity)
{
	//Calculates if provided entity is close to player
	//We calculate cell indexes and take the smaller difference between them
	//Then we compare it to player's field of view range
	//This is used so we do not spam events to EntityRenderSystem recalculations every time some entity moves.
	auto& player = mSystemContext.entityManager.getPlayer();
	return Utilities::isEntityWithinFOVRange(player, entity);
}

void VisibilitySystem::notifyFadeSystemWithFOVChanges()
{
	std::unordered_set<Tile*> tilesToSend;

	for (Tile* tile : mTilesInFieldOfView)
	{
		const auto color = tile->tileVisual.getFillColor();

		if (tile->visible && color != Utilities::getVisibleTileColor(*tile))
			tilesToSend.insert(tile);
	}
	for (Tile* tile : mPreviousFOV)
	{
		const auto color = tile->tileVisual.getFillColor();
		if (!tile->visible && tile->explored && color != Utilities::getExploredTileColor(*tile))
			tilesToSend.insert(tile);
	}
	mSystemContext.eventManager.notify<TileFadeRequestEvent>(TileFadeRequestEvent(std::move(tilesToSend)));
}

void VisibilitySystem::notifyMonsterAppeared(Entity& entity)
{
	mSystemContext.eventManager.notify<MonsterAppearedEvent>(MonsterAppearedEvent(entity));
}

void VisibilitySystem::notifyMonsterDisappeared(Entity& entity)
{
	mSystemContext.eventManager.notify<MonsterDisappearedEvent>(MonsterDisappearedEvent(entity));
}

void VisibilitySystem::computeShadowcastFOV()
{
	transitionFOVToExplored();
	auto& player = mSystemContext.entityManager.getPlayer();
	auto cell = Utilities::getEntityCell(player);
	auto radius = player.getComponent<FieldOfViewComponent>().cRangeOfView;
	addTileToCurrentFov(cell.x, cell.y);

	for (int i = 0; i < 8; ++i)
		doShadowcast(cell, 1, 0, 1, radius, i);
}

void VisibilitySystem::doShadowcast(const sf::Vector2i& fromCell, int row, double start, double end, int maxRadius, int octant)
{
	/*
	* Code idea taken from: 
	* https://gist.github.com/370417/59bb06ced7e740e11ec7dda9d82717f6
	*/

	auto& tiles = mTileMap.getTiles();
	if (row > maxRadius || start >= end)
		return;

	double xMin = std::round((row - 0.5) * start);
	double xMax = std::ceil((row + 0.5) * end - 0.5);
	auto& transform = mOctantTransforms[octant];

	for (int x = static_cast<int>(xMin); x <= static_cast<int>(xMax); ++x)
	{
		int realX = fromCell.x + transform.xx * x + transform.xy * row;
		int realY = fromCell.y + transform.yx * x + transform.yy * row;

		if (!mTileMap.isBlockingSight(realX, realY))
		{
			if (x >= row * start && x <= row * end)
				addTileToCurrentFov(realX, realY);
		}
		else
		{
			if(x >= (row - 0.5) * start && x - 0.5 <= row * end)
				addTileToCurrentFov(realX, realY);

			doShadowcast(fromCell, row + 1, start, (x - 0.5) / row, maxRadius, octant);
			start = (x + 0.5) / row;
			if (start >= end)
				return;
		}
	}

	doShadowcast(fromCell, row + 1, start, end, maxRadius, octant);
}


