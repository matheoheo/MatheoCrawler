#include "pch.h"
#include "MenuState.h"
#include "Config.h"
#include "StateIdentifiers.h"
#include "Utilities.h"

MenuState::MenuState(GameContext& gameContext)
	:IState(gameContext),
	mBackground(gameContext.textures.get(TextureIdentifier::MenuBackground))
{
	mMenuView.setSize(Config::fWindowSize);
	mMenuView.setCenter(Config::fWindowSize * 0.5f);
	gameContext.window.setView(mMenuView);

	gameContext.eventManager.notify<ClearNonGlobalEvents>(ClearNonGlobalEvents());
}

void MenuState::onEnter()
{
	createButtons();
	createBackgroundSprite();
}

void MenuState::processEvents(sf::Event event)
{
	handleButtonClick(event);
}

void MenuState::update(const sf::Time& deltaTime)
{
	IState::updateMousePosition();
	updateButtons();
}

void MenuState::render()
{
	renderButtons();
}

void MenuState::createButtons()
{
	const auto& font = mGameContext.fonts.get(FontIdentifiers::Default);
	const auto charSize = Config::getCharacterSize();
	constexpr size_t buttonCount = 5;
	mButtons.reserve(buttonCount);
	
	const std::array<TextButton::TextButtonInitData, buttonCount> buttonsData =
	{ {
		{"Start New Game", [this]() {
			proceedToState(StateIdentifier::GameState, true);
		}},
		{"Options", [this]() {
			proceedToState(StateIdentifier::OptionsState, false);
		}},
		{"Credits", [this]() {
			proceedToState(StateIdentifier::CreditsState, false);
		}},
		{"About",   [this]() {
			proceedToState(StateIdentifier::AboutState, false);
		}},
		{"Exit",    [this]() {
			mGameContext.window.close();
		}}
	} };
	constexpr float paddingRatio = 0.075f;
	sf::Vector2f pos{ Config::fWindowSize.x * paddingRatio, Config::fWindowSize.y * paddingRatio };
	const float margin = charSize + charSize * 0.3f;

	for (const auto& data : buttonsData)
	{
		mButtons.emplace_back(font, data.label, charSize, pos);
		mButtons.back().setCallback(data.callback);
		pos.y += margin;
	}
}

void MenuState::updateButtons()
{
	for (auto& button : mButtons)
		button.update(fMousePos);
}

void MenuState::handleButtonClick(const sf::Event event)
{
	for (auto& button : mButtons)
	{
		if (button.isPressed(event))
		{
			button.invoke();
			return;
		}
	}
}

void MenuState::renderButtons()
{
	mGameContext.window.draw(mBackground);

	for (const auto& button : mButtons)
		button.render(mGameContext.window);
}

void MenuState::createBackgroundSprite()
{
	Utilities::scaleSprite(mBackground, Config::fWindowSize);
}

void MenuState::proceedToState(StateIdentifier id, bool popPrevious)
{
	mGameContext.eventManager.notify<SwitchStateEvent>(SwitchStateEvent(id, popPrevious));
}
