#pragma once
#include "IState.h"
#include "TextButton.h"

class CreditsState : public IState
{
public:
	CreditsState(GameContext& gameContext);
	// Inherited via IState
	virtual void onEnter() override;
	virtual void processEvents(sf::Event event) override;
	virtual void update(const sf::Time& deltaTime) override;
	virtual void render() override;
private:
	void createBackground();
	void createHeaderText();
	void createBackButton();
	void createNextPageButton();
	void createButtons();
	void createLPCAssetsPage();
	void createAllElsePage();
	void createPages();

	void addOnePage(const std::string& str);
private:
	struct CreditsPage
	{
		std::vector<sf::Text> creditsLines;
	};

	sf::Vector2f getPagePos() const;
	float getMaxPageWidth() const;
private:
	const sf::Font& mFont;
	const unsigned int mCharSize;
	sf::Sprite mBackground;
	sf::Text mHeaderText;
	TextButton mBackButton;
	TextButton mNextPageButton;
	std::vector<TextButton*> mButtons;
	std::vector<CreditsPage> mPages;
	size_t mCurrentPageIndex;
	static constexpr size_t MaxPages = 2;
};