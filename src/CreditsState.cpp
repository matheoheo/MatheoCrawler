#include "pch.h"
#include "CreditsState.h"
#include "Config.h"
#include "Utilities.h"

CreditsState::CreditsState(GameContext& gameContext)
	:IState(gameContext),
	mFont(mGameContext.fonts.get(FontIdentifiers::UIFont)),
	mCharSize(Config::getCharacterSize() / 2),
	mBackground(gameContext.textures.get(TextureIdentifier::CreditsBackground)),
	mHeaderText(mGameContext.fonts.get(FontIdentifiers::Default)),
	mBackButton(mGameContext.fonts.get(FontIdentifiers::Default), "Go Back", mCharSize * 2),
	mNextPageButton(mGameContext.fonts.get(FontIdentifiers::Default), "Next Credits", mCharSize * 2),
	mCurrentPageIndex(0)
{
	mPages.reserve(MaxPages);
}

void CreditsState::onEnter()
{
	createBackground();
	createHeaderText();
	createButtons();
	createPages();
}

void CreditsState::processEvents(sf::Event event)
{
	for (const auto& btn : mButtons)
	{
		if (btn->isPressed(event))
		{
			btn->invoke();
			return;
		}
	}
}

void CreditsState::update(const sf::Time& deltaTime)
{
	IState::updateMousePosition();
	for (const auto& btn : mButtons)
	{
		btn->update(fMousePos);
	}
}

void CreditsState::render()
{
	mGameContext.window.draw(mBackground);
	mGameContext.window.draw(mHeaderText);
	for (const auto& btn : mButtons)
		btn->render(mGameContext.window);

	const auto& page = mPages[mCurrentPageIndex];
	for (const auto& line : page.creditsLines)
		mGameContext.window.draw(line);
}

void CreditsState::createBackground()
{
	Utilities::scaleSprite(mBackground, Config::fWindowSize);
}

void CreditsState::createHeaderText()
{
	mHeaderText = Utilities::createStateHeaderText(mGameContext.fonts.get(FontIdentifiers::Default), "Crawler's Credits");
}

void CreditsState::createBackButton()
{
	mBackButton.setCallback([this]()
		{
			mGameContext.eventManager.notify<PopStateEvent>(PopStateEvent());
		});

	mBackButton.setPosition(Utilities::getSecondaryButtonsPos());
}

void CreditsState::createNextPageButton()
{
	mNextPageButton.setCallback([this]()
		{
			mCurrentPageIndex = (mCurrentPageIndex + 1) % mPages.size();
			std::cout << "Page index: " << mCurrentPageIndex << '\n';
		});

	mNextPageButton.setPosition(Utilities::getPrimaryButtonPos());
}

void CreditsState::createButtons()
{
	createBackButton();
	createNextPageButton();

	mButtons.push_back(&mBackButton);
	mButtons.push_back(&mNextPageButton);
}

void CreditsState::createLPCAssetsPage()
{
	const std::string lpcCredits = 
	 R"(The credits below are for graphical assets (player and monsters) used in this  project.

Sprites by : Johannes Sjölund(wulax), Michael Whitlock(bigbeargames), Matthew Krohn(makrohn), Nila122, David Conway Jr. (JaidynReiman),
Carlo Enrico Victoria(Nemisys), Thane Brimhall(pennomi), laetissima, bluecarrot16, Luke Mehl, Benjamin K.Smith(BenCreating), MuffinElZangano, Durrani, kheftel,
Stephen Challener(Redshrike), William.Thompsonj, Marcel van de Steeg(MadMarcel), TheraHedwig, Evert, Pierre Vigier(pvigier), Eliza Wyatt(ElizaWy), Johannes Sjölund(wulax),
Sander Frenken(castelonia), dalonedrau, Lanea Zimmerman(Sharm), Manuel Riecke(MrBeast), Barbara Riviera, Joe White, Mandi Paugh, Shaun Williams, Daniel Eddeland(daneeklu), 
Emilio J.Sanchez - Sierra, drjamgo, gr3yh47, tskaufma, Fabzy, Yamilian, Skorpio, kheftel, Tuomo Untinen(reemax), Tracy, thecilekli, LordNeo, Stafford McIntyre, PlatForge project,
DCSS authors, DarkwallLKE, Charles Sanchez(CharlesGabriel), Radomir Dopieralski, macmanmatty, Cobra Hubbard(BlueVortexGames), Inboxninja, kcilds / Rocetti / Eredah, Napsio(Vitruvian Studio),
The Foreman, AntumDeluge

Sprites contributed as part of the Liberated Pixel Cup project from OpenGameArt.org : http://opengameart.org/content/lpc-collection
License : Creative Commons Attribution - ShareAlike 3.0 (CC - BY - SA 3.0) http://creativecommons.org/licenses/by-sa/3.0/
Detailed credits : Available in the game files in /assets folder
Github link to LPC: https://github.com/liberatedpixelcup/Universal-LPC-Spritesheet-Character-Generator/blob/master/README.md)";

	addOnePage(lpcCredits);
}

void CreditsState::createAllElsePage()
{
	const std::string otherCredits = R"(The following credits cover additional assets, fonts, and resources used in this game.
Assets / Icons:
- Some icons presented in the game are by J. W. Bjerk (eleazzaar) -- www.jwbjerk.com/art.

Game Development:
- Developed by MatheoHeo using C++ & SFML 3.0

AI Assistance:
- Certain assets, color choices, and design decisions were assisted by AI tools, including ChatGPT, Gemini (Google), and Microsoft Designer.

Fonts:
- [Dungeon Font]   - Source: [https://www.dafont.com/dungeon-sn.font] - (license unknown)
- [Unitblock Font] - Source: [https://www.fontspace.com/unitblock-font-f104834] - (Public Domain)

Links:
- Developer GitHub: [https://github.com/matheoheo]
- Developer YouTube: [https://www.youtube.com/@MatheoHeo]
- SFML Library: [https://www.sfml-dev.org/]

Licenses:
- OGA-BY 3.0, CC-BY-SA 3.0, GPL 3.0, CC-BY 4.0. Full licenses texts available online.
)";
	
	addOnePage(otherCredits);
}

void CreditsState::createPages()
{
	createLPCAssetsPage();
	createAllElsePage();

	constexpr sf::Color textColor{ 220, 220, 220 };
	auto pagePos = getPagePos();

	for(auto& page : mPages)
	{
		for (auto& line : page.creditsLines)
		{
			line.setPosition(pagePos);
			line.setFillColor(textColor);
		}
	}
}

void CreditsState::addOnePage(const std::string& str)
{
	float maxWidth = getMaxPageWidth();
	std::string wrappedText = Utilities::wrapText(str, mFont, maxWidth, mCharSize);
	CreditsPage page;
	page.creditsLines.emplace_back(mFont, wrappedText, mCharSize);
	mPages.push_back(std::move(page));
}

sf::Vector2f CreditsState::getPagePos() const
{
	auto headerBounds = mHeaderText.getGlobalBounds();
	const float xPos = Config::fWindowSize.x * 0.1f; //starts at 10% of screen
	const float yPos = headerBounds.position.y + headerBounds.size.y + mCharSize;

	return sf::Vector2f(xPos, yPos);
}

float CreditsState::getMaxPageWidth() const
{
	return Config::fWindowSize.x * 0.8f;
}
