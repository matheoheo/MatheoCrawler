#pragma once
#include "GameContext.h"
class Entity;

class UIComponent
{
public:
	UIComponent(GameContext& gameContext, Entity& player);

	virtual void processEvents(const sf::Event event) = 0;
	virtual void update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime) = 0;
	virtual void render() = 0;
protected:
	GameContext& mGameContext;
	Entity& player;
};

