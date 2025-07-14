#pragma once
#include "TextButton.h"

class OptionSelector
{
public:
	OptionSelector(const sf::Font& font, const std::string& label);
	void init();
	void addValue(const std::string& option);
	void processClick(const sf::Event event);
	void update(const sf::Vector2f& mousePos);
	void render(sf::RenderWindow& window);

	void setLabelPosition(const sf::Vector2f& pos);
	void setValuePosition(const sf::Vector2f& pos);
	void setArrowPosition(const sf::Vector2f& pos);

	void setPositions(const sf::Vector2f& labelPos, const sf::Vector2f& valuePos, const sf::Vector2f& arrowPos);

	//returns currently picked value
	const std::string& getValue() const;
	//returns width of label Text
	float getLabelWidth() const;
	//returns width of longest value.
	float getLongestValueWidth();
private:
	void selectNext();
	void selectPrevious();
	void updateValueText();
private:
	const unsigned int mCharSize;
	sf::Text mLabel;
	sf::Text mValue;
	TextButton mLeftArrow;
	TextButton mRightArrow;
	std::vector<std::string> mValueOptions;
	size_t mCurrValueIndex;

	std::vector<TextButton*> mButtons;
};

