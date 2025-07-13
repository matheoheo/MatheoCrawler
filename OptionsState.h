#pragma once
#include "IState.h"
#include "TextButton.h"

class OptionsState :
    public IState
{
public:
    OptionsState(GameContext& gameContext);
    // Inherited via IState
    virtual void onEnter() override;
    virtual void processEvents(sf::Event event) override;
    virtual void update(const sf::Time& deltaTime) override;
    virtual void render() override;
private:
    void createBackButton();
    void createBackground();
    void createOptionsText();
private:
    const unsigned int mCharSize;
    TextButton mBackButton;
    sf::Sprite mBackground;
    sf::Text mOptionsText;
};

