#pragma once
#include "IState.h"
#include "TextButton.h"

class MenuState :
    public IState
{
public:
    MenuState(GameContext& gameContext);
    // Inherited via IState
    virtual void onEnter() override;
    virtual void processEvents(sf::Event event) override;
    virtual void update(const sf::Time& deltaTime) override;
    virtual void render() override;
private:
    void createButtons();
    void updateButtons();
    void handleButtonClick(const sf::Event event);
    void renderButtons();

    void createBackgroundSprite();
    void proceedToState(StateIdentifier id, bool popPrevious);
private:
    std::vector<TextButton> mButtons;
    sf::Sprite mBackground;
};

