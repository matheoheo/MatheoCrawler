#include "pch.h"
#include "StateManager.h"
#include "GameState.h"
#include "StateIdentifiers.h"
#include "LoadingState.h"
#include "MenuState.h"
#include "OptionsState.h"
#include "CreditsState.h"
#include "AboutState.h"

StateManager::StateManager(GameContext& gameContext)
    :mGameContext(gameContext),
    mReRegisterEvents(false)
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

    if (mReRegisterEvents)
    {
        //See ClearNonGlobalEvents in GameEvents.h to see why this is here.
        mReRegisterEvents = false;
        registerToEvents();
    }
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
    registerToClearNonGlobalEvents();
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
            pushState(std::make_unique<LoadingState>(mGameContext, data.tasksVec));
        });
}

void StateManager::registerToClearNonGlobalEvents()
{
    mGameContext.eventManager.registerEvent<ClearNonGlobalEvents>([this](const ClearNonGlobalEvents& data)
        {
            mReRegisterEvents = true;
            mGameContext.eventManager.clearAllEvents();
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
        pushState(std::make_unique<MenuState>(mGameContext));
        break;
    case StateIdentifier::GameState:
        pushState(std::make_unique<GameState>(mGameContext));
        break;
    case StateIdentifier::OptionsState:
        pushState(std::make_unique<OptionsState>(mGameContext));
        break;
    case StateIdentifier::CreditsState:
        pushState(std::make_unique<CreditsState>(mGameContext));
        break;
    case StateIdentifier::AboutState:
        pushState(std::make_unique<AboutState>(mGameContext));
        break;
    default:
        break;
    }
}
