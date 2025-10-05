#pragma once
#include "IAboutSection.h"

class GameOverviewSection : public IAboutSection
{
public:
	GameOverviewSection(GameContext& gameContext, const sf::Vector2f& sectionPos, const sf::Vector2f& sectionSize);

	// Inherited via IAboutSection
	virtual void processEvents(sf::Event event) override;
	virtual void update(const sf::Vector2f& fMousePos, const sf::Time& deltaTime) override;
	virtual void render() override;
private:
	void createOverviewText();
	void createControlsText();
	void createSeparators();
private:
	struct TableLine
	{
		sf::Text left;
		sf::Text right;

		TableLine(const sf::Font& font)
			:left(font),
			right(font) {}
	};

	void createTableEntry(const std::string& leftStr, const std::string& rightStr);
	void createTableData();
	void positionTableData();
	void renderTableLines();
	float getLeftTableX() const;
	float getRightTableX() const;
	float getPosYBetweenLines(const TableLine& l1, const TableLine& l2) const;
private:
	float getLeftColumnWidth() const;
	float getRightColumnPosX() const;
	float getRightColumnWidth() const;
private:
	sf::Text mOverviewText;
	sf::Text mControlsText;
	std::vector<TableLine> mTableLines;
	std::vector<sf::RectangleShape> mSeparators;
};