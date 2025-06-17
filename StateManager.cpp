#include "pch.h"
#include "StateManager.h"
#include "GameState.h"
#include "StateIdentifiers.h"
#include "LoadingState.h"

StateManager::StateManager(GameContext& gameContext)
    :mGameContext(gameContext)
{
    registerToEvents();
}

void StateManager::processEvents(sf::Event event)
{
    if (isTopStackValid())
        mStateStack.top()->processEvents(event);
}

void StateManager::update(const sf::Time& deltaTime)
{
    if (isTopStackValid())
        mStateStack.top()->update(deltaTime);
}

void StateManager::render()
{
    if (isTopStackValid())
        mStateStack.top()->render();
}

void StateManager::registerToEvents()
{
    registerToSwitchStateEvent();
    registerToPopStateEvent();
    registerToEnterLoadingStateEvent();
}

void StateManager::registerToSwitchStateEvent()
{
    mGameContext.eventManager.registerEvent<SwitchStateEvent>([this](const SwitchStateEvent& data)
        {
            if (data.popPrevious)
                popState();
            switchState(data.stateId);
        });
}

void StateManager::registerToPopStateEvent()
{
    mGameContext.eventManager.registerEvent<PopStateEvent>([this](const PopStateEvent& data)
        {
            popState();
        });
}

void StateManager::registerToEnterLoadingStateEvent()
{
    mGameContext.eventManager.registerEvent<EnterLoadingStateEvent>([this](const EnterLoadingStateEvent& data)
        {
            pushState(std::make_unique<LoadingState>(mGameContext, std::move(data.tasksVec)));
        });
}

bool StateManager::isTopStackValid() const
{
    return !mStateStack.empty() && mStateStack.top();
}

void StateManager::pushState(StatePtr state)
{
    mStateStack.push(std::move(state));
    if (isTopStackValid())
        mStateStack.top()->onEnter();
}

void StateManager::popState()
{
    if (!mStateStack.empty())
        mStateStack.pop();
}

void StateManager::switchState(StateIdentifier stateId)
{
    switch (stateId)
    {
    case StateIdentifier::MenuState:
        break;
    case StateIdentifier::GameState:
        pushState(std::make_unique<GameState>(mGameContext));
        break;
    default:
        break;
    }
}
