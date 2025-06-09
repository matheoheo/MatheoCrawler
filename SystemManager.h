#pragma once
#include "ISystem.h"

class SystemManager
{
public:
	using SystemPtr = std::unique_ptr<ISystem>;

	void addSystem(SystemPtr ptr);
	void processEvents(const sf::Event event);
	void update(const sf::Time& deltaTime);
	void render(sf::RenderWindow& window);
private:
	std::vector<SystemPtr> mSystems;
};

