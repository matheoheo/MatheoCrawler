#include "pch.h"
#include "Engine.h"
#include "Config.h"
#include "StateIdentifiers.h"
Engine::Engine()
	:mWindow(sf::VideoMode(Config::windowSize), "Matheo Crawler", sf::Style::Titlebar | sf::Style::Close,
		Config::fullscreen ? sf::State::Fullscreen : sf::State::Windowed),
	mGameContext(mWindow, mTextures, mFonts, mEventManager),
	mStateManager(mGameContext)
{
}

void Engine::start()
{
	mGameContext.eventManager.notify<SwitchStateEvent>(SwitchStateEvent(StateIdentifier::GameState, false));
	using namespace std::chrono;

	sf::Clock fpsClock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	sf::Time timePerFrame(sf::seconds(1.f / 60.f));

	while (mWindow.isOpen())
	{
		timeSinceLastUpdate += fpsClock.restart();

		auto start = steady_clock::now();

		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents();
			update(timePerFrame);
		}
		render();
		auto end = steady_clock::now();
		auto duration = duration_cast<milliseconds>(end - start);
		auto t = duration.count();
		if(t >= 15)
			std::cout << "Function took " << duration.count() << " ms\n";
	}
}

void Engine::processEvents()
{
	while (const auto event = mWindow.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
			mWindow.close();
		mStateManager.processEvents(event.value());
	}
}

void Engine::update(const sf::Time& deltaTime)
{
	mStateManager.update(deltaTime);
}

void Engine::render()
{
	mWindow.clear();
	mStateManager.render();
	mWindow.display();
}

void Engine::preloadTextures()
{
	mTextures.load(TextureIdentifier::LoadingScreen, "assets/loading/loadingScreen.png");
	mTextures.load(TextureIdentifier::MattLogo,		 "assets/logo/mattGamesLogo.jpeg");
	mTextures.load(TextureIdentifier::Player,		 "assets/entities/player/player.png");
	mTextures.load(TextureIdentifier::Moranna,       "assets/entities/Moranna/moranna.png");
	mTextures.load(TextureIdentifier::Skletorus,	 "assets/entities/skeleton_axe/skeleton.png");
	mTextures.load(TextureIdentifier::Bonvik,		 "assets/entities/Bonvik/bonvik.png");
	mTextures.load(TextureIdentifier::Suash,		 "assets/ui/suash/suashIcon.png");
	mTextures.load(TextureIdentifier::Twarf,		 "assets/ui/twarf/twarfIcon.png");
	mTextures.load(TextureIdentifier::Devoyer,		 "assets/ui/attIcons/devoyer.png");
	mTextures.load(TextureIdentifier::QuickHealIcon, "assets/ui/spellIcons/healing/QuickHeal.png");
	mTextures.load(TextureIdentifier::MajorHealIcon, "assets/ui/spellIcons/healing/MajorHeal.png");
	mTextures.load(TextureIdentifier::HealthRegenIcon, "assets/ui/spellIcons/healing/HealthRegen.png");
	mTextures.load(TextureIdentifier::ManaRegenIcon, "assets/ui/spellIcons/healing/ManaRegen.png");
	mTextures.load(TextureIdentifier::WaterballIcon, "assets/ui/spellIcons/projectiles/waterball.png");
	mTextures.load(TextureIdentifier::PureProjIcon, "assets/ui/spellIcons/projectiles/pure.png");
	mTextures.load(TextureIdentifier::FireballIcon, "assets/ui/spellIcons/projectiles/fireball.png");
	mTextures.load(TextureIdentifier::BloodballIcon, "assets/ui/spellIcons/projectiles/bloodball.png");


	mTextures.load(TextureIdentifier::WaterBall0,    "assets/entities/projectiles/waterball/Water__05.png");
	mTextures.load(TextureIdentifier::PureProjectile0, "assets/entities/projectiles/pure/Pure_08.png");
	mTextures.load(TextureIdentifier::Fireball0,  "assets/entities/projectiles/fireball/Fireball_01.png");
	mTextures.load(TextureIdentifier::Bloodball0, "assets/entities/projectiles/bloodball/Blood_01.png");



	mFonts.load(FontIdentifiers::Default, "assets/font/gameFont/dungeonFont.ttf");
	mFonts.load(FontIdentifiers::UIFont, "assets/font/uiFont/unitblockFont.ttf");

}
