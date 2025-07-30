#include "pch.h"
#include "ISystem.h"

ISystem::ISystem(SystemContext& systemContext)
	:mSystemContext(systemContext)
{
	registerToRemoveEntityFromSystemEvent();
}

void ISystem::processEvents(const sf::Event event)
{
}

void ISystem::render(sf::RenderWindow& window)
{
}

bool ISystem::isEntityAlreadyTracked(const Entity& entity) 
{
	auto it = std::ranges::find_if(mTrackedEntities, [&entity](const Entity* e)
		{
			return e->getEntityId() == entity.getEntityId();
		});

	return it != std::ranges::end(mTrackedEntities);
}

void ISystem::registerToRemoveEntityFromSystemEvent()
{
	mSystemContext.eventManager.registerEvent<RemoveEntityFromSystemEvent>([this](const RemoveEntityFromSystemEvent& data)
		{
			std::erase(mTrackedEntities, &data.entity);
		});
}
