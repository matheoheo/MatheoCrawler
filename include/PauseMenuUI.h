#pragma once
#include "UIComponent.h"
#include "TextButton.h"

class PauseMenuUI : public UIComponent
{
public:
	PauseMenuUI(GameContext& gameContext, Entity& player);

	virtual void processEvents(const sf::Event event);
	virtual void update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime);
	virtual void render();
private:
	void createOverlay();
	void createGameMenuText(unsigned int charSize);
	void createButtons();

	void proceedToMainMenu();
	void quitGame();
private:
	bool mIsActive;
	const sf::Keyboard::Key mPauseKey;
	sf::RectangleShape mOverlay;
	sf::Text mGameMenuText;
	std::vector<TextButton> mButtons;
};

