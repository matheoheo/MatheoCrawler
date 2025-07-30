#pragma once

class EventManager;
class EntityManager;
class TileMap;

struct BehaviorContext
{
	EventManager& eventManager;
	EntityManager& entityManager;
	TileMap& tileMap;

	BehaviorContext(EventManager& eventManager, EntityManager& entityManager, TileMap& tileMap)
		:eventManager(eventManager),
		entityManager(entityManager),
		tileMap(tileMap)
	{

	}
};

