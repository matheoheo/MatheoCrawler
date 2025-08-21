#pragma once
#include "UIComponent.h"

class GameOverUI : public UIComponent
{
public:
	GameOverUI(GameContext& gameContext, Entity& player);

	// Inherited via UIComponent
	virtual void processEvents(const sf::Event event) override;
	virtual void update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime) override;
	virtual void render() override;
private:
	void registerToEvents();
	void registerToPlayerRunEndedEvent();

	void createBackground();
	void createYouDiedText();
	bool hasFadeTimePassed() const;
	float getFadeProgress() const;

	void fadeInBackground();
private:
	struct StatisticLine
	{
		sf::Text label;
		sf::Text value;
		StatisticLine(const sf::Font& font, unsigned int charSize)
			:label(font, "", charSize),
			value(font, "", charSize) {}
	};
	void createStatisticLines(const StatisticMap& stats);
	void createOneLine(const Statistic& statistic, const sf::Vector2f& labelPos, const sf::Vector2f& valuePos);
	void createLeftLinesColumn(const StatisticMap& stats, float posY, float linesMargin, float labelValueMargin);
	void createRightLinesColumn(const StatisticMap& stats, float posY, float linesMargin, float labelValueMargin);
	void createLinesColumn(const StatisticMap& stats, std::span<const StatisticType> data, float posX, float posY, float linesMargin, float labelValueMargin);

	float getMaxWidthOfLabel(const StatisticMap& stats, std::span<const StatisticType> data) const;
private:
	const sf::Font& mFont;
	const unsigned int mLineCharSize;
	sf::RectangleShape mBackground;
	sf::Text mYouDiedText;
	std::vector<StatisticLine> mLines;
	bool mGameOverActive;
	int mTimeSinceActive;
	const int mFadeTime;
};