#pragma once
#include "IState.h"
#include <thread>
#include <atomic>
class LoadingState :
    public IState
{
public:
    using LoaderTaskVec = std::vector<std::function<void()>>;
    LoadingState(GameContext& gameContext, LoaderTaskVec taskQueue);
    // Inherited via IState
    virtual void onEnter() override;
    virtual void processEvents(sf::Event event) override;
    virtual void update(const sf::Time& deltaTime) override;
    virtual void render() override;
private:
    void createLoadingScreen();
    void updateLoadingScreen();
    void renderLoadingScreen();

    void runTasks();
private:
    sf::Sprite mLoadingSprite;
    sf::Sprite mGameLogo;
    sf::Text mLoadingText;
    std::vector<std::string> mLoadingStrings;
    size_t mStringIndex;
    size_t mElaspedFramesCount;
    std::jthread mLoadingThread;
    std::atomic<bool> mLoadingFinished;
    LoaderTaskVec mTasks;
};

