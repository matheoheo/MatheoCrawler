#include "pch.h"
#include "PauseMenuUI.h"
#include "Config.h"
#include "Utilities.h"
#include "StateIdentifiers.h"

PauseMenuUI::PauseMenuUI(GameContext& gameContext, Entity& player)
	:UIComponent(gameContext, player),
	mIsActive(false),
	mPauseKey(sf::Keyboard::Key::Escape),
	mGameMenuText(gameContext.fonts.get(FontIdentifiers::UIFont))
{
	createOverlay();
	createButtons();
}

void PauseMenuUI::processEvents(const sf::Event event)
{
	if (const auto data = event.getIf<sf::Event::KeyPressed>())
	{
		if (data->code == mPauseKey)
			mIsActive = !mIsActive;
	}
	for (auto& btn : mButtons)
	{
		if (btn.isPressed(event))
		{
			btn.invoke();
			return;
		}
	}
}

void PauseMenuUI::update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime)
{
	if (!mIsActive)
		return;

	for (auto& btn : mButtons)
	{
		btn.update(mousePosition);
	}
}

void PauseMenuUI::render()
{
	if (!mIsActive)
		return;

	mGameContext.window.draw(mOverlay);
	mGameContext.window.draw(mGameMenuText);
	for (const auto& btn : mButtons)
		btn.render(mGameContext.window);
}

void PauseMenuUI::createOverlay()
{
	constexpr sf::Color overlayColor{ 20, 20, 20, 220 };
	constexpr sf::Color borderColor{ 200,40, 40 };
	constexpr float borderThickness = 2.f;
	const sf::Vector2f size{
		Config::fWindowSize.x * 0.15f, //15% of width
		Config::fWindowSize.y * 0.20f //20% of height
	};
	const sf::Vector2f pos{ Config::fWindowSize * 0.5f }; 
	const sf::Vector2f origin{ size * 0.5f };

	mOverlay.setFillColor(overlayColor);
	mOverlay.setOutlineColor(borderColor);
	mOverlay.setOutlineThickness(borderThickness);
	mOverlay.setSize(size);
	mOverlay.setOrigin(origin);
	mOverlay.setPosition(pos);
}

void PauseMenuUI::createGameMenuText(unsigned int charSize)
{
	mGameMenuText.setCharacterSize(charSize);
	mGameMenuText.setString("Game Menu");
	mGameMenuText.setFillColor({ 230, 230, 230 });
	Utilities::setTextOriginOnCenter(mGameMenuText);

	const int margin = static_cast<int>(charSize * 0.2);
	const auto overlayPos = mOverlay.getPosition();
	const auto overlayOrigin = mOverlay.getOrigin();

	const sf::Vector2f textPos{
		overlayPos.x,
		overlayPos.y - overlayOrigin.y + margin
	};

	mGameMenuText.setPosition(textPos);
}

void PauseMenuUI::createButtons()
{
	constexpr size_t buttonsCount = 3;
	mButtons.reserve(buttonsCount);
	const auto& font = mGameContext.fonts.get(FontIdentifiers::UIFont);
	const auto charSize = Config::getCharacterSize() / 2;
	createGameMenuText(charSize);
	const float posX = mGameMenuText.getPosition().x;
	float posY = mGameMenuText.getPosition().y + charSize * 2; //multiply by 2 to get bigger space between text and first button.
	const float margin = charSize * 0.5f;

	const std::array<TextButton::TextButtonInitData, buttonsCount> initData =
	{ {
		{.label = "Continue",          .callback = [this]() {mIsActive = false; }},
		{.label = "Back to Main Menu", .callback = [this]() {proceedToMainMenu(); }},
		{.label = "Quit Game",         .callback = [this]() {quitGame(); }}
	} };

	for (const auto& data : initData)
	{
		mButtons.emplace_back(font, data.label, charSize);
		mButtons.back().setOriginOnCenter();
		mButtons.back().setPosition({ posX, posY });
		mButtons.back().setCallback(data.callback);
		posY += static_cast<float>(charSize) + margin;
	}
}

void PauseMenuUI::proceedToMainMenu()
{
	mGameContext.eventManager.notify<ProceedToMainMenuEvent>(ProceedToMainMenuEvent());
}

void PauseMenuUI::quitGame()
{
	mGameContext.window.close();
}
