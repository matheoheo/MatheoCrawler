#pragma once
#include "IState.h"
#include "TextButton.h"
#include "IAboutSection.h"
#include "ConceptDefinitions.h"

class AboutState : public IState
{
public:
	AboutState(GameContext& gameContext);
	// Inherited via IState
	virtual void onEnter() override;
	virtual void processEvents(sf::Event event) override;
	virtual void update(const sf::Time& deltaTime) override;
	virtual void render() override;
private:
	void createBackground();
	void createBackButton();
	void createHeaderText();
	void createSectionsButtons();
	void createOverline();
	void positionOverline(const TextButton* btn);
	void positionSectionsButtons();
	void calculateSectionLayout();
	

	template<SectionConcept Section>
	void changeSection();
private:
	const sf::Font& mFont;
	const unsigned int mCharSize;
	sf::Sprite mBackground;
	std::vector<TextButton> mButtons;
	sf::Text mHeaderText;
	sf::Vector2f mSectionPos;
	sf::Vector2f mSectionSize;
	std::unique_ptr<IAboutSection> mCurrSection;
	sf::RectangleShape mOverline;
};

template<SectionConcept Section>
inline void AboutState::changeSection()
{
	mCurrSection = std::make_unique<Section>(mGameContext, mSectionPos, mSectionSize);
}
