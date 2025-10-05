#include "pch.h"
#include "IAboutSection.h"
#include "Config.h"

IAboutSection::IAboutSection(GameContext& gameContext, const sf::Vector2f& sectionPos, const sf::Vector2f& sectionSize)
	:mGameContext(gameContext),
	mSectionPos(sectionPos),
	mSectionSize(sectionSize),
	mFont(gameContext.fonts.get(FontIdentifiers::UIFont)),
	mCharSize(static_cast<unsigned int>(Config::getCharacterSize() / 1.5f)),
	mTextColor(sf::Color::White)
{
}
