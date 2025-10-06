#include "pch.h"
#include "GameOverUI.h"
#include "Config.h"
#include "Utilities.h"

GameOverUI::GameOverUI(GameContext& gameContext, Entity& player)
	:UIComponent(gameContext, player),
	mFont(gameContext.fonts.get(FontIdentifiers::UIFont)),
	mLineCharSize(Config::getCharacterSize() / 2),
	mYouDiedText(mFont),
	mGameOverActive(false),
	mTimeSinceActive(0),
	mFadeTime(2000),
	mProceedButton(mFont, "Proceed to Main Menu", mLineCharSize)
{
	createBackground();
	createYouDiedText();
	registerToEvents();
}

void GameOverUI::processEvents(const sf::Event event)
{
	if (!hasFadeTimePassed())
		return;

	if (mProceedButton.isPressed(event))
	{
		mProceedButton.invoke();
		return;
	}
}

void GameOverUI::update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime)
{
	if (mGameOverActive)
	{
		if(!hasFadeTimePassed())
		{
			mTimeSinceActive += deltaTime.asMilliseconds();
			fadeInBackground();
		}
		mProceedButton.update(mousePosition);
	}
}

void GameOverUI::render()
{
	if(mGameOverActive)
		mGameContext.window.draw(mBackground);

	if (!hasFadeTimePassed())
		return;
	mGameContext.window.draw(mYouDiedText);

	for (const auto& line : mLines)
	{
		mGameContext.window.draw(line.label);
		mGameContext.window.draw(line.value);
	}

	mProceedButton.render(mGameContext.window);
}

void GameOverUI::registerToEvents()
{
	registerToPlayerRunEndedEvent();
}

void GameOverUI::registerToPlayerRunEndedEvent()
{
	mGameContext.eventManager.registerEvent<PlayerRunEndedEvent>([this](const PlayerRunEndedEvent& data)
		{
			if (mGameOverActive)
			return;

			createStatisticLines(data.statsMap);
			createProceedToMenuButton();
			mGameOverActive = true;
		});
}

void GameOverUI::createBackground()
{
	mBackground.setSize(Config::fWindowSize);
	mBackground.setFillColor({ 0, 0, 0, 0 });
}

void GameOverUI::createYouDiedText()
{
	mYouDiedText.setCharacterSize(Config::getCharacterSize() * 2);
	mYouDiedText.setFillColor({ 200, 40, 40 });
	mYouDiedText.setString("YOU DIED");
	Utilities::setTextOriginOnCenter(mYouDiedText);
	const float xPos = Config::fWindowSize.x * 0.5f;
	const float yPos = Config::fWindowSize.y * 0.05f;
	mYouDiedText.setPosition({ xPos, yPos });
}

bool GameOverUI::hasFadeTimePassed() const
{
	return mTimeSinceActive >= mFadeTime;
}

float GameOverUI::getFadeProgress() const
{
	float progress = static_cast<float>(mTimeSinceActive) / static_cast<float>(mFadeTime);
	return std::clamp(progress, 0.f, 1.0f);
}

void GameOverUI::fadeInBackground()
{
	constexpr std::uint8_t desiredAlpha = 240;
	float progress = getFadeProgress();
	const std::uint8_t currAlpha = static_cast<std::uint8_t>(progress * desiredAlpha);

	auto color = mBackground.getFillColor();
	color.a = currAlpha;
	mBackground.setFillColor(color);
}

void GameOverUI::createStatisticLines(const StatisticMap& stats)
{
	mLines.clear();

	const float posY = mYouDiedText.getPosition().y + mYouDiedText.getGlobalBounds().size.y * 2;
	const float linesMargin = mLineCharSize * 0.5f;
	const float labelValueMargin = mLineCharSize * 2.f;

	createLeftLinesColumn(stats, posY, linesMargin, labelValueMargin);
	createRightLinesColumn(stats, posY, linesMargin, labelValueMargin);
}

void GameOverUI::createOneLine(const Statistic& statistic, const sf::Vector2f& labelPos, const sf::Vector2f& valuePos)
{
	constexpr sf::Color labelColor{200, 200, 200};
	constexpr sf::Color valueColor{ 80, 245, 230 };
	
	mLines.emplace_back(mFont, mLineCharSize);
	std::string valStr = std::to_string(statistic.value);

	if (statistic.type && *statistic.type == StatisticType::TimeSurvived)
		valStr = formatTime(statistic.value);

	auto& line = mLines.back();
	line.label.setString(statistic.label);
	line.label.setFillColor(labelColor);
	line.label.setPosition(labelPos);
	line.value.setString(valStr);
	line.value.setFillColor(valueColor);
	line.value.setPosition(valuePos);
}

void GameOverUI::createLeftLinesColumn(const StatisticMap& stats, float posY, float linesMargin, float labelValueMargin)
{
	constexpr size_t count = 5;
	constexpr std::array<StatisticType, count> leftCol =
	{
		StatisticType::GoldCollected, StatisticType::EnemiesDefeated, 
		StatisticType::TotalDamageDealt, StatisticType::HitsDealt, StatisticType::AvgDamageDealt
	};

	const float labelXPos = Config::fWindowSize.x * 0.25f;
	createLinesColumn(stats, leftCol, labelXPos, posY, linesMargin, labelValueMargin);
}

void GameOverUI::createRightLinesColumn(const StatisticMap& stats, float posY, float linesMargin, float labelValueMargin)
{
	constexpr size_t count = 5;
	constexpr std::array<StatisticType, count> rightCol =
	{
		StatisticType::StepsTaken, StatisticType::TimeSurvived, 
		StatisticType::TotalDamageTaken, StatisticType::HitsTaken, StatisticType::AvgDamageTaken
	};

	const float labelXPos = Config::fWindowSize.x * 0.6f;
	createLinesColumn(stats, rightCol, labelXPos, posY, linesMargin, labelValueMargin);
}

void GameOverUI::createLinesColumn(const StatisticMap& stats, std::span<const StatisticType> data, float posX, float posY, float linesMargin, float labelValueMargin)
{
	const float maxWidth = getMaxWidthOfLabel(stats, data);
	const float valueXPos = posX + maxWidth + labelValueMargin;

	for (auto stat : data)
	{
		if (auto it = stats.find(stat); it != std::end(stats))
		{
			createOneLine(it->second, { posX, posY }, { valueXPos, posY });
			posY += mLineCharSize + linesMargin;
		}
	}
}

float GameOverUI::getMaxWidthOfLabel(const StatisticMap& stats, std::span<const StatisticType> data) const
{
	std::vector<std::reference_wrapper<const Statistic>> referenceData;
	for (const auto stat : data)
	{
		if (stats.contains(stat))
			referenceData.push_back(std::cref(stats.at(stat)));
	}

	float width = std::numeric_limits<float>::min();
	for (const auto stat : referenceData)
	{
		sf::Text text(mFont, stat.get().label, mLineCharSize);
		float textWidth = text.getGlobalBounds().size.x;
		if (width < textWidth)
			width = textWidth;
	}

	return width;
}

std::string GameOverUI::formatTime(int timeInMs) const
{
	int seconds = timeInMs / 1000;
	int minutes = seconds / 60;
	seconds %= 60;
	int hours = minutes / 60;
	minutes %= 60;

	if (hours == 0)
		return std::format("{:02}m{:02}s", minutes, seconds);

	return std::format("{}h{:02}m{:02}s", hours, minutes, seconds);
}

void GameOverUI::createProceedToMenuButton()
{
	mProceedButton.setOriginOnCenter();
	const auto lastBounds = mLines.back().label.getGlobalBounds();
	const float xPos = Config::fWindowSize.x * 0.5f;
	const float yPos = lastBounds.position.y + lastBounds.size.y + mLineCharSize * 3;
	mProceedButton.setPosition({ xPos, yPos });

	mProceedButton.setCallback([this]()
		{
			mGameContext.eventManager.notify<ProceedToMainMenuEvent>(ProceedToMainMenuEvent());
		});
}
