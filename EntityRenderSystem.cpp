#include "pch.h"
#include "EntityRenderSystem.h"
#include "Tile.h"

EntityRenderSystem::EntityRenderSystem(SystemContext& systemContext)
	:ISystem(systemContext),
	mFramesSinceLastRecalculation(0)
{
	registerToEvents();
}

void EntityRenderSystem::update(const sf::Time& deltaTime)
{
	//we dont want to recalculate every frame - and that might happen if many monsters move frequently
	++mFramesSinceLastRecalculation;
	if (mFramesSinceLastRecalculation > 100000)
		mFramesSinceLastRecalculation = 50;
}

void EntityRenderSystem::render(sf::RenderWindow& window)
{
	for (const Entity* ent : mRenderedEntities)
	{
		if (ent)
		{
			auto& sprite = ent->getComponent<SpriteComponent>().cSprite;
			window.draw(sprite);
		}
	}
}

void EntityRenderSystem::registerToEvents()
{
	registerToUpdateEntityRenderTilesEvent();
}

void EntityRenderSystem::registerToUpdateEntityRenderTilesEvent()
{
	mSystemContext.eventManager.registerEvent<UpdateEntityRenderTilesEvent>([this](const UpdateEntityRenderTilesEvent& data)
		{
			if (mFramesSinceLastRecalculation < 6)
				return;

			determineRenderedEntities(data.vec);
			mFramesSinceLastRecalculation = 0;
		});
}

void EntityRenderSystem::determineRenderedEntities(const std::vector<Tile*>& tiles)
{
	std::unordered_set<Entity*> set;

	for (const auto& tile : tiles)
	{
		if (tile->tileType != TileType::Floor || !tile->visible)
			continue;

		if (tile->reservedEntity)
			set.insert(tile->reservedEntity);

		for (auto& ent : tile->occupyingEntities)
			set.insert(ent);
	}
	mRenderedEntities.assign(std::begin(set), std::end(set));
}
