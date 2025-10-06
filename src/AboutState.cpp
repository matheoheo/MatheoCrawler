#include "pch.h"
#include "AboutState.h"
#include "Config.h"
#include "Utilities.h"
#include "GameOverviewSection.h"
#include "BattleAndMagicSection.h"
#include "PlayerProgressSection.h"

AboutState::AboutState(GameContext& gameContext)
	:IState(gameContext),
	mFont(gameContext.fonts.get(FontIdentifiers::Default)),
	mCharSize(Config::getCharacterSize()),
	mBackground(gameContext.textures.get(TextureIdentifier::AboutBackground)),
	mHeaderText(mFont)
{
}

void AboutState::onEnter()
{
	createBackground();
	createBackButton();
	createHeaderText();
	createSectionsButtons();
}

void AboutState::processEvents(sf::Event event)
{
	for (auto& button : mButtons)
	{
		if (button.isPressed(event))
		{
			button.invoke();
			return;
		}
	}

	if (mCurrSection)
		mCurrSection->processEvents(event);
}

void AboutState::update(const sf::Time& deltaTime)
{
	IState::updateMousePosition();
	for (auto& button : mButtons)
	{
		button.update(fMousePos);
	}

	if (mCurrSection)
		mCurrSection->update(fMousePos, deltaTime);
}

void AboutState::render()
{
	mGameContext.window.draw(mBackground);
	mGameContext.window.draw(mHeaderText);

	for (const auto& button : mButtons)
		button.render(mGameContext.window);

	//mGameContext.window.draw(rect);
	if (mCurrSection)
		mCurrSection->render();
}

void AboutState::createBackground()
{
	Utilities::scaleSprite(mBackground, Config::fWindowSize);
}

void AboutState::createBackButton()
{
	auto& backButton = mButtons.emplace_back(mFont, "Go Back", mCharSize, Utilities::getSecondaryButtonsPos());

	backButton.setCallback([this]()
		{
			mGameContext.eventManager.notify<PopStateEvent>(PopStateEvent());
		});
}

void AboutState::createHeaderText()
{
	mHeaderText = Utilities::createStateHeaderText(mFont, "About Crawler");
}

void AboutState::createSectionsButtons()
{
	const std::array<TextButton::TextButtonInitData, 4> btnsData =
	{ {
		{"Game Overview",   [this]() {
			changeSection<GameOverviewSection>();
	}},
		{"Battle & Magic",  [this]() {
			changeSection<BattleAndMagicSection>();
	}},
		{"Player Progress", [this]() {
			changeSection<PlayerProgressSection>();
	}},
		{"Dungeon Foes",    [this]() {
	
	}}
	} };
	auto charSize = Config::getCharacterSize();
	
	for (const auto& data : btnsData)
	{
		auto& btn = mButtons.emplace_back(mFont, data.label, mCharSize);
		btn.setCallback(data.callback);
	}
	positionSectionsButtons();
	calculateSectionLayout();
}

void AboutState::positionSectionsButtons()
{
	//Sections names are of similar length, thats why im gonna use length of first section
	const float sectionWidth = mButtons[1].getSize().x;
	const float totalWidth = Config::fWindowSize.x;
	const float sectionCount = static_cast<float>(mButtons.size() - 1); //-1 because there is also Go Back button.
	const float spacing = (totalWidth - (sectionWidth * sectionCount)) / (sectionCount + 1);
	
	auto headerBounds = mHeaderText.getGlobalBounds();
	const float y = headerBounds.position.y + headerBounds.size.y + (headerBounds.size.y * 0.35f);
	float x = spacing;
	for (size_t i = 1; i < mButtons.size(); ++i) //start from 1, because 0 is backButton
	{
		mButtons[i].setPosition({ x, y });
		x += sectionWidth + spacing;
	}
}

void AboutState::calculateSectionLayout()
{
	const TextButton& backBtn = mButtons[0];
	const TextButton& firstBtn = mButtons[1];
	const TextButton& lastBtn = mButtons.back();
	constexpr float marginPercent = 0.04f;
	const float xMargin = Config::fWindowSize.x * marginPercent;
	const float yMargin = Config::fWindowSize.y * marginPercent;
	auto firstSize = firstBtn.getSize();
	auto lastSize = lastBtn.getSize();

	mSectionPos = {
		firstBtn.getPosition().x + xMargin,
		lastBtn.getPosition().y + lastSize.y + yMargin
	};

	mSectionSize = {
		(lastBtn.getPosition().x + lastSize.x - xMargin) - mSectionPos.x,
		(backBtn.getPosition().y - yMargin) - mSectionPos.y
	};

	rect.setPosition(mSectionPos);
	rect.setSize(mSectionSize);

	rect.setFillColor(sf::Color::Transparent);
	rect.setOutlineColor(sf::Color::White);
	rect.setOutlineThickness(2.f);
}
