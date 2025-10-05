#pragma once
#include "GameContext.h"

class IAboutSection
{
public:
	IAboutSection(GameContext& gameContext, const sf::Vector2f& sectionPos, const sf::Vector2f& sectionSize);

	virtual ~IAboutSection() = default;
	virtual void processEvents(sf::Event event) = 0;
	virtual void update(const sf::Vector2f& fMousePos, const sf::Time& deltaTime) = 0;
	virtual void render() = 0;
protected:
	GameContext& mGameContext;
	const sf::Vector2f mSectionPos;
	const sf::Vector2f mSectionSize;
	const sf::Font& mFont;
	const unsigned int mCharSize;
	const sf::Color mTextColor;
};