#pragma once
#include "ISystem.h"
#include "TileMap.h"

class VisibilitySystem : public ISystem
{
public:
	VisibilitySystem(SystemContext& systemContext, TileMap& tileMap, const sf::View& gameView);
	// Inherited via ISystem
	virtual void update(const sf::Time& deltaTime) override;
private:
	struct CellBounds {
		sf::Vector2i min;
		sf::Vector2i max;
	};

	void registerToEvents();
	void registerToPlayerMoveFinishedEvent();
	void registerToMoveAllowedEvent();
	void registerToEntityFinishedMoveEvent();
	void regToRemoveEntityFromSystemEvent();

	void collectTilesInCameraView();
	CellBounds getCellBoundsInCameraView() const;
	CellBounds getCellBoundsInRange(const sf::Vector2f& fromPos, const sf::Vector2i& range) const;

	//sets tiles in mTilesInFieldOfView vector to explored and not visible, also clears vector.
	void transitionFOVToExplored();
	void addTileToCurrentFov(Tile& tile);
	void addTileToCurrentFov(int x, int y);
	bool isValidFOVTile(const Tile& tile);

	void calculateVisibleEntities();
	void sendUpdateEntityRenderTilesEvent();
	bool isEntityCloseToPlayer(const Entity& entity);
	void notifyFadeSystemWithFOVChanges();
	void notifyMonsterAppeared(Entity& entity);
	void notifyMonsterDisappeared(Entity& entity);
private:
	struct OctantTransform {
		int xx, xy, yx, yy;
	};
	void computeShadowcastFOV();
	void doShadowcast(const sf::Vector2i& fromCell, int row, double start, double end, int maxRadius, int octant);
private:
	TileMap& mTileMap;
	const sf::View& mGameView;
	std::vector<Tile*> mTilesInFieldOfView; //we track tiles in field of view to render only those entities who are visible to player.
	std::vector<Tile*> mPreviousFOV;
	bool mShouldRecalculateEntityVisibility; //this variable delegates calculations to next frame.
	std::unordered_set<Entity*> mLastVisibleEntities; //tracking which monsters appear or disappear.
	std::vector<OctantTransform> mOctantTransforms;
};

