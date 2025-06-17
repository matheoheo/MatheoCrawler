#pragma once
#include "IState.h"
#include <stack>

class StateManager
{
public:
	using StatePtr = std::unique_ptr<IState>;
	StateManager(GameContext& gameContext);
	void processEvents(sf::Event event);
	void update(const sf::Time& deltaTime);
	void render();
private:
	void registerToEvents();
	void registerToSwitchStateEvent();
	void registerToPopStateEvent();
	void registerToEnterLoadingStateEvent();
	bool isTopStackValid() const;

	void pushState(StatePtr state);
	void popState();
	void switchState(StateIdentifier stateId);
private:
	GameContext& mGameContext;
	std::stack<StatePtr> mStateStack;
};

