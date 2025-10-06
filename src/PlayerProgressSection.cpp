#include "pch.h"
#include "PlayerProgressSection.h"
#include "Utilities.h"

PlayerProgressSection::PlayerProgressSection(GameContext& gameContext, const sf::Vector2f& sectionPos, const sf::Vector2f& sectionSize)
	:IAboutSection(gameContext, sectionPos, sectionSize),
	mSectionText(mFont, "", mCharSize)
{
	createSectionText();
}

void PlayerProgressSection::processEvents(sf::Event event)
{
}

void PlayerProgressSection::update(const sf::Vector2f& fMousePos, const sf::Time& deltaTime)
{
}

void PlayerProgressSection::render()
{
	mGameContext.window.draw(mSectionText);
}

void PlayerProgressSection::createSectionText()
{
	const std::string sectionStr =
		R"(Every victory makes you stronger. Defeated enemies drop gold. Gold servers as your main resource for growth - it's what shapes your character's power and adopt to challenges ahead.

You can spend gold in upgrade shopy by pressing M. There you can enhance your core statistics. Each upgrade makes noticable difference in combat, letting you attack faster, survive longer and resist damage more effectively.

Progress isn't just about raw stats - it's also about improving your attack types and magic. New spells can be puruchased and upgraded, expanding your combat options and playstyle. You must choose carefully which paths to invest in, balancing strength and magic to match your preffered approach to battle.

Attack types can be modified to strengthen your sword swings. Choose wisely!)";

	std::string wrapped = Utilities::wrapText(sectionStr, mFont, mSectionSize.x, mCharSize);

	mSectionText.setPosition(mSectionPos);
	mSectionText.setString(wrapped);
}
