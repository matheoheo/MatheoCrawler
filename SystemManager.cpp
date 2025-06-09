#include "pch.h"
#include "SystemManager.h"

void SystemManager::addSystem(SystemPtr ptr)
{
	mSystems.push_back(std::move(ptr));
}

void SystemManager::processEvents(const sf::Event event)
{
	for (auto& system : mSystems)
		system->processEvents(event);
}

void SystemManager::update(const sf::Time& deltaTime)
{
	for (auto& system : mSystems)
		system->update(deltaTime);
}

void SystemManager::render(sf::RenderWindow& window)
{
	for (auto& system : mSystems)
		system->render(window);
}
