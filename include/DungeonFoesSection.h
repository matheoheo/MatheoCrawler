#pragma once
#include "IAboutSection.h"

class DungeonFoesSection : public IAboutSection
{
public:
	DungeonFoesSection(GameContext& gameContext, const sf::Vector2f& sectionPos, const sf::Vector2f& sectionSize);

	// Inherited via IAboutSection
	virtual void processEvents(sf::Event event) override;
	virtual void update(const sf::Vector2f& fMousePos, const sf::Time& deltaTime) override;
	virtual void render() override;
private:
	struct FoeEntry
	{
		sf::Sprite sprite;
		sf::Text name;
		sf::FloatRect bounds;
		std::string summaryStr;
		FoeEntry(const sf::Texture& texture, const sf::Font& font, unsigned int charSize)
			:sprite(texture),
			name(font, "", charSize)
		{}
	};
	void createFoeEntry(const sf::Texture& texture, const std::string& nameStr, const std::string& summaryStr);
	void createEntries();
	void positionEntries();
	void setEntiresBounds();
private:
	void createHighlighter();
	void updateHighlighter(const sf::Vector2f& mousePos);
	void updateOnClick();
	void determineSummaryPos();
	void createSummaryOnClick();
private:
	std::vector<FoeEntry> mFoeEntries;
	sf::Text mSummary;
	sf::Vector2f mIconSize;
	sf::Vector2f mSummaryPos;
	float yMargin;
	sf::RectangleShape mHighlighter;
	sf::RectangleShape mSelected;
	FoeEntry* mLastClicked;
};