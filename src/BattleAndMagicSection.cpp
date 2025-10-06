#include "pch.h"
#include "BattleAndMagicSection.h"
#include "Utilities.h"

BattleAndMagicSection::BattleAndMagicSection(GameContext& gameContext, const sf::Vector2f& sectionPos, const sf::Vector2f& sectionSize)
	:IAboutSection(gameContext, sectionPos, sectionSize),
	mSectionText(mFont, "", mCharSize)
{
	createSectionText();
}

void BattleAndMagicSection::processEvents(sf::Event event)
{
}

void BattleAndMagicSection::update(const sf::Vector2f& fMousePos, const sf::Time& deltaTime)
{
}

void BattleAndMagicSection::render()
{
	mGameContext.window.draw(mSectionText);
}

void BattleAndMagicSection::createSectionText()
{
	const std::string sectionStr =
		R"(Combat takes place in real time on a tile-based grid, where every move shapes the flow of battle. You can't turn in place - to face your enemy, you must move.
Positioning is crucial; staying aware of your orientation can decide wether your strike lands or you get hit instead.

Your weapon offers 3 attack types: one swings forward and to the left, another forward and to the right, and the last focuses directly ahead. Each attack has it's own rythm, controlled by your attack speed. You must commit to every strike before acting again, so timing and placement matter just as much as raw strength.

Magic brings power and versatility to your fights. Spells come with casting times, cooldowns and mana costs requiring carefull resource managment. You'll wield offensive spells such as projectiles and area attacks, quick healings and regeneration spells that boost your natural recovery over time. Relying too heavily on magic can drain your mana, leaving you vulnerable - so every cast should count.)";

	std::string wrapped = Utilities::wrapText(sectionStr, mFont, mSectionSize.x, mCharSize);

	mSectionText.setPosition(mSectionPos);
	mSectionText.setString(wrapped);
}
