#pragma once
#include "SystemContext.h"

class ISystem
{
public:
	ISystem(SystemContext& systemContext);
	virtual ~ISystem() = default;

	virtual void processEvents(const sf::Event event);
	virtual void update(const sf::Time& deltaTime) = 0;
	virtual void render(sf::RenderWindow& window);
protected:
	bool isEntityAlreadyTracked(const Entity& entity) ;
protected:
	SystemContext& mSystemContext;
	std::vector<Entity*> mTrackedEntities;
};

