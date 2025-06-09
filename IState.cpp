#include "pch.h"
#include "IState.h"

IState::IState(GameContext& gameContext)
	:mGameContext(gameContext)
{
}

void IState::updateMousePosition()
{
	auto pos = sf::Mouse::getPosition(mGameContext.window);
	fMousePos = mGameContext.window.mapPixelToCoords(pos);
}
