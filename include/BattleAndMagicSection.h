#pragma once
#include "IAboutSection.h"

class BattleAndMagicSection 
	:public IAboutSection
{
public:
	BattleAndMagicSection(GameContext& gameContext, const sf::Vector2f& sectionPos, const sf::Vector2f& sectionSize);

	// Inherited via IAboutSection
	virtual void processEvents(sf::Event event) override;
	virtual void update(const sf::Vector2f& fMousePos, const sf::Time& deltaTime) override;
	virtual void render() override;
private:
	void createSectionText();
private:
	sf::Text mSectionText;
};