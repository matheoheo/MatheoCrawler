#pragma once
#include "GameContext.h"

//interface for every state of application
class IState
{
public:
	IState(GameContext& gameContext);
	virtual ~IState() = default;
	virtual void onEnter() = 0;
	virtual void processEvents(sf::Event event) = 0;
	virtual void update(const sf::Time& deltaTime) = 0;
	virtual void render() = 0;
protected:
	void updateMousePosition();
protected:
	GameContext& mGameContext;
	sf::Vector2f fMousePos;
};

