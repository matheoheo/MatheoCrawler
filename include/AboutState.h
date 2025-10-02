#pragma once
#include "IState.h"

class AboutState : public IState
{
public:
	AboutState(GameContext& gameContext);
	// Inherited via IState
	virtual void onEnter() override;
	virtual void processEvents(sf::Event event) override;
	virtual void update(const sf::Time& deltaTime) override;
	virtual void render() override;
private:

private:
};