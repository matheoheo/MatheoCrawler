#include "pch.h"
#include "EntityDeathSystem.h"
#include "Utilities.h"

EntityDeathSystem::EntityDeathSystem(SystemContext& systemContext)
	:ISystem(systemContext)
{
	registerToEvents();
}

void EntityDeathSystem::update(const sf::Time& deltaTime)
{
	for (const auto& entity : mDeadEntities)
	{
		markEntityAsDead(*entity);
		mFinishedEntities.push_back(entity->getEntityId());
	}
	removeFinishedEntities();
}

void EntityDeathSystem::registerToEvents()
{
	mSystemContext.eventManager.registerEvent<EntityDiedEvent>([this](const EntityDiedEvent& data)
		{
			mDeadEntities.push_back(&data.entity);
			//markEntityAsDead(data.entity);
		});
}

void EntityDeathSystem::markEntityAsDead(Entity& entity)
{
	auto entityPos = Utilities::getEntityVisualPosition(entity);
	auto& evMng = mSystemContext.eventManager;

	evMng.notify<TileVacatedEvent>(TileVacatedEvent(entity, entityPos));
	evMng.notify<ReserveTileEvent>(ReserveTileEvent(nullptr, entityPos));
	evMng.notify<RemoveEntityFromSystemEvent>(RemoveEntityFromSystemEvent(entity));
}

void EntityDeathSystem::removeFinishedEntities()
{
	mDeadEntities.clear();
	for (const auto& id : mFinishedEntities)
		mSystemContext.entityManager.removeEntity(id);
	mFinishedEntities.clear();
}
