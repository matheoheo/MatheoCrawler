#pragma once
#include "EventManager.h"
#include "EntityManager.h"

struct SystemContext
{
	EventManager& eventManager;
	EntityManager& entityManager;
	SystemContext(EventManager& eventManager, EntityManager& entityManager)
		:eventManager(eventManager),
		entityManager(entityManager)
	{

	}
};

